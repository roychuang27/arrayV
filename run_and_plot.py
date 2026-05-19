import argparse
import glob
import os
import platform
import shutil
import subprocess
import sys
import tempfile

import imageio.v2 as imageio
import numpy as np
import pandas as pd
from moviepy import AudioFileClip, VideoFileClip
from PIL import Image, ImageDraw
from scipy.io.wavfile import write as write_wav
from tqdm import tqdm


COLOR_MAP = {
    "swap": (255, 0, 0),
    "compare": (255, 255, 0),
    "sorted": (0, 255, 0),
    "idle": (255, 255, 255),
}

DEFAULT_WIDTH = 512
DEFAULT_HEIGHT = 384
DEFAULT_SAMPLERATE = 44100
FPS_CHOICES = [24, 30, 60, 120]


def list_cpp_files():
    return sorted(glob.glob("*.cpp"))


def _fallback_menu(title, options):
    print(title)
    for i, opt in enumerate(options, 1):
        print(f"{i}. {opt}")

    while True:
        choice = input("Select a number: ").strip()
        try:
            idx = int(choice) - 1
            if 0 <= idx < len(options):
                return idx
        except ValueError:
            pass
        print("Invalid selection.")


def select_option_tui(title, options, default_idx=0):
    if not options:
        raise ValueError("No options provided.")

    if not sys.stdin.isatty() or not sys.stdout.isatty():
        return _fallback_menu(title, options)

    try:
        import curses
    except Exception:
        return _fallback_menu(title, options)

    def _menu(stdscr):
        curses.curs_set(0)
        idx = max(0, min(default_idx, len(options) - 1))

        while True:
            stdscr.erase()
            h, w = stdscr.getmaxyx()

            header = f"{title}  (↑/↓ or j/k, Enter to confirm, q to quit)"
            stdscr.addnstr(0, 0, header, w - 1)

            for i, opt in enumerate(options):
                y = 2 + i
                if y >= h - 1:
                    break

                line = f"> {opt}" if i == idx else f"  {opt}"
                if i == idx:
                    stdscr.attron(curses.A_REVERSE)
                    stdscr.addnstr(y, 0, line, w - 1)
                    stdscr.attroff(curses.A_REVERSE)
                else:
                    stdscr.addnstr(y, 0, line, w - 1)

            stdscr.refresh()
            key = stdscr.getch()

            if key in (curses.KEY_UP, ord("k")):
                idx = (idx - 1) % len(options)
            elif key in (curses.KEY_DOWN, ord("j")):
                idx = (idx + 1) % len(options)
            elif key in (10, 13, curses.KEY_ENTER):
                return idx
            elif key in (27, ord("q")):
                raise SystemExit("Selection canceled.")

    return curses.wrapper(_menu)


def prompt_text_tui(title, default_text):
    if not sys.stdin.isatty() or not sys.stdout.isatty():
        text = input(f"{title} [{default_text}]: ").strip()
        return text or default_text

    try:
        import curses
    except Exception:
        text = input(f"{title} [{default_text}]: ").strip()
        return text or default_text

    def _prompt(stdscr):
        curses.curs_set(1)
        stdscr.erase()
        h, w = stdscr.getmaxyx()

        stdscr.addnstr(0, 0, title, w - 1)
        stdscr.addnstr(2, 0, f"Press Enter after typing. Blank = {default_text}", w - 1)
        stdscr.addnstr(4, 0, "> ", w - 1)
        stdscr.refresh()

        curses.echo()
        try:
            raw = stdscr.getstr(4, 2, max(1, w - 3))
            text = raw.decode("utf-8", "ignore").strip()
        finally:
            curses.noecho()

        return text or default_text

    return curses.wrapper(_prompt)


def select_cpp_file_tui():
    cpp_files = list_cpp_files()
    if not cpp_files:
        raise FileNotFoundError("No .cpp files found in the current directory.")

    idx = select_option_tui("Select a C++ source file", cpp_files, default_idx=0)
    return cpp_files[idx]


def select_preview_tui():
    idx = select_option_tui("Preview final video?", ["No", "Yes"], default_idx=0)
    return idx == 1


def select_fps_tui():
    labels = [str(x) for x in FPS_CHOICES]
    idx = select_option_tui("Select FPS", labels, default_idx=1)
    return FPS_CHOICES[idx]


def select_output_name_tui(stem):
    mode = select_option_tui(
        "Select output name",
        [
            f"Use source name: {stem}.mp4",
            "Enter custom name",
        ],
        default_idx=0,
    )

    if mode == 0:
        return f"{stem}.mp4"

    custom = prompt_text_tui("Enter output filename", f"{stem}.mp4")
    if not custom.lower().endswith(".mp4"):
        custom += ".mp4"
    return custom


def generate_tone(
    frequency=440,
    duration=0.1,
    samplerate=DEFAULT_SAMPLERATE,
    waveform="sine",
    amplitude=0.03,
):
    t = np.linspace(0, duration, int(samplerate * duration), endpoint=False)

    if waveform == "square":
        wave = np.sign(np.sin(2 * np.pi * frequency * t))
    else:
        wave = np.sin(2 * np.pi * frequency * t)

    envelope = np.exp(-40 * t)
    audio = wave * envelope * amplitude
    return (audio * 32767).astype(np.int16)


def generate_compare_beep(frequency, duration, samplerate):
    return generate_tone(
        frequency=frequency,
        duration=duration,
        samplerate=samplerate,
        waveform="sine",
    )


def generate_swap_beep(frequency, duration, samplerate):
    return generate_tone(
        frequency=frequency,
        duration=duration,
        samplerate=samplerate,
        waveform="square",
    )


def get_frequency_from_values(val1, val2):
    avg = (val1 + val2) / 2
    return 220 + (avg / 100) * 660


def parse_active_row(cell):
    if cell is None:
        return "idle", []

    text = str(cell).strip()
    if not text:
        return "idle", []

    parts = [x.strip() for x in text.split(",") if x.strip()]
    action = parts[0].lower()

    indices = []
    for x in parts[1:]:
        try:
            indices.append(int(x))
        except ValueError:
            pass

    return action, indices


def load_trace_data():
    trace = pd.read_csv("trace.csv", header=None)

    try:
        with open("active.csv", "r", encoding="utf-8") as f:
            active_lines = [line.strip() for line in f.readlines()]
    except FileNotFoundError:
        active_lines = ["idle"] * len(trace)

    return trace, active_lines


def draw_step_array(
    values,
    action_type="idle",
    indices=None,
    width=DEFAULT_WIDTH,
    height=DEFAULT_HEIGHT,
):
    if not values:
        return np.zeros((height, width, 3), dtype=np.uint8)

    n = len(values)
    bar_width = max(1, width // n)

    max_val = max(values)
    if max_val == 0:
        max_val = 1

    img = Image.new("RGB", (width, height), "black")
    draw = ImageDraw.Draw(img)

    indices_set = set(indices or [])

    for i, value in enumerate(values):
        x0 = i * bar_width
        x1 = min(width - 1, x0 + bar_width - 1)

        bar_height = int((value / max_val) * height)
        y0 = height - bar_height
        y1 = height

        color = (
            COLOR_MAP[action_type]
            if action_type in COLOR_MAP and i in indices_set
            else COLOR_MAP["idle"]
        )

        draw.rectangle([x0, y0, x1, y1], fill=color)

    return np.array(img)


def generate_frames(trace, active_lines):
    frames = []
    total_frames = min(len(trace), len(active_lines))

    for i in tqdm(range(total_frames), desc="Generating frames", unit="frame"):
        values = trace.iloc[i].values.astype(int).tolist()
        action, indices = parse_active_row(active_lines[i])
        frames.append(draw_step_array(values, action_type=action, indices=indices))

    if len(trace) > 0:
        final_values = trace.iloc[-1].values.astype(int).tolist()
        frames.append(
            draw_step_array(
                final_values,
                action_type="sorted",
                indices=list(range(len(final_values))),
            )
        )

    return frames


def render_video(frames, output_path, fps):
    writer = imageio.get_writer(output_path, fps=fps, codec="libx264")
    try:
        for frame in tqdm(frames, desc="Writing MP4", unit="frame"):
            writer.append_data(frame)
    finally:
        writer.close()


def render_audio(
    trace,
    active_lines,
    fps,
    output_path="beep.wav",
    samplerate=DEFAULT_SAMPLERATE,
):
    frame_duration = 1.0 / fps
    beeps = []
    total_frames = min(len(trace), len(active_lines))

    for i in tqdm(range(total_frames), desc="Generating audio", unit="frame"):
        values = trace.iloc[i].values.astype(int).tolist()
        action, indices = parse_active_row(active_lines[i])

        valid_pair = (
            len(indices) == 2
            and 0 <= indices[0] < len(values)
            and 0 <= indices[1] < len(values)
        )

        if action == "compare" and valid_pair:
            val1 = values[indices[0]]
            val2 = values[indices[1]]
            freq = get_frequency_from_values(val1, val2)
            beep = generate_compare_beep(freq, frame_duration, samplerate)
        elif action == "swap" and valid_pair:
            val1 = values[indices[0]]
            val2 = values[indices[1]]
            freq = get_frequency_from_values(val1, val2)
            beep = generate_swap_beep(freq, frame_duration, samplerate)
        else:
            beep = np.zeros(int(samplerate * frame_duration), dtype=np.int16)

        beeps.append(beep)

    beeps.append(np.zeros(int(samplerate * frame_duration), dtype=np.int16))
    full_audio = np.concatenate(beeps) if beeps else np.zeros(1, dtype=np.int16)

    expected_samples = int(samplerate * len(beeps) / fps)
    if len(full_audio) < expected_samples:
        pad = np.zeros(expected_samples - len(full_audio), dtype=np.int16)
        full_audio = np.concatenate([full_audio, pad])

    write_wav(output_path, samplerate, full_audio)
    return output_path


def merge_av(video_path, audio_path, output_path):
    video = VideoFileClip(video_path)
    audio = AudioFileClip(audio_path)

    try:
        if hasattr(audio, "with_duration"):
            audio = audio.with_duration(video.duration)
        else:
            audio = audio.set_duration(video.duration)

        if hasattr(video, "with_audio"):
            final_video = video.with_audio(audio)
        else:
            final_video = video.set_audio(audio)

        final_video.write_videofile(
            output_path,
            codec="libx264",
            audio_codec="aac",
            logger="bar",
        )
    finally:
        video.close()
        audio.close()


def find_cpp_compiler():
    for compiler in ("g++", "clang++"):
        path = shutil.which(compiler)
        if path:
            return path
    raise FileNotFoundError(
        "No C++ compiler found. Install g++ or clang++ and make sure it is on PATH."
    )


def build_cpp_source(source_path, output_path):
    if not os.path.exists(source_path):
        raise FileNotFoundError(f"Source file not found: {source_path}")

    compiler = find_cpp_compiler()
    cmd = [
        compiler,
        source_path,
        "-O2",
        "-std=c++17",
        "-o",
        output_path,
    ]

    print(f"Building {cmd}")
    subprocess.run(cmd, check=True)


def run_binary(binary_path):
    print(f"Running {binary_path}")
    subprocess.run([binary_path], check=True)


def build_and_run_cpp(source_path):
    stem = os.path.splitext(os.path.basename(source_path))[0]

    if platform.system() == "Windows":
        binary_name = f"{stem}.exe"
        run_path = binary_name
    else:
        binary_name = f"{stem}.out"
        run_path = f"./{binary_name}"

    build_cpp_source(source_path, binary_name)
    run_binary(run_path)
    return run_path


def preview_video(filename):
    if not os.path.exists(filename):
        print(f"File not found: {filename}")
        return

    system = platform.system()

    try:
        if system == "Windows":
            os.startfile(filename)
        elif system == "Darwin":
            subprocess.run(["open", filename], check=False)
        elif system == "Linux":
            subprocess.run(["xdg-open", filename], check=False)
        else:
            print(f"Unsupported OS: {system}")
    except Exception as exc:
        print(f"Failed to preview: {exc}")


def make_temp_path(suffix):
    fd, path = tempfile.mkstemp(prefix="arrayv_", suffix=suffix)
    os.close(fd)
    return path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cpp-file", type=str)
    parser.add_argument("--preview", action=argparse.BooleanOptionalAction, default=None)
    parser.add_argument("--fps", type=int)
    parser.add_argument("--output", type=str)
    args = parser.parse_args()

    if args.cpp_file:
        cpp_file = args.cpp_file
    else:
        cpp_file = select_cpp_file_tui()

    stem = os.path.splitext(os.path.basename(cpp_file))[0]

    if args.preview is None:
        preview = select_preview_tui()
    else:
        preview = args.preview

    if args.fps is None:
        fps = select_fps_tui()
    else:
        fps = args.fps

    if args.output is None:
        final_output = select_output_name_tui(stem)
    else:
        final_output = args.output

    if not final_output.lower().endswith(".mp4"):
        final_output += ".mp4"

    build_and_run_cpp(cpp_file)

    trace, active_lines = load_trace_data()
    frames = generate_frames(trace, active_lines)

    temp_video = make_temp_path(".mp4")
    temp_audio = make_temp_path(".wav")

    try:
        render_video(frames, temp_video, fps)
        render_audio(trace, active_lines, fps=fps, output_path=temp_audio)

        if os.path.exists(final_output):
            os.remove(final_output)

        merge_av(temp_video, temp_audio, final_output)
    finally:
        for path in (temp_video, temp_audio):
            try:
                if os.path.exists(path):
                    os.remove(path)
            except OSError:
                pass

    if preview:
        preview_video(final_output)


if __name__ == "__main__":
    main()
