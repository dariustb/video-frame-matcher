#!/usr/bin/env python3
import re
import shutil
import sys
from pathlib import Path

import cv2
import numpy as np
from PySide6.QtCore import QProcess, Qt
from PySide6.QtGui import QFont, QImage, QPixmap
from PySide6.QtWidgets import (
    QApplication,
    QDoubleSpinBox,
    QFileDialog,
    QFormLayout,
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMainWindow,
    QPlainTextEdit,
    QPushButton,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

VFM_DEFAULT_THRESHOLD = 0.85
VFM_DEFAULT_FRAME_STEP = 2
VFM_DEFAULT_START_TIME = 0.0

PREVIEW_MAX_W = 400
PREVIEW_MAX_H = 300

_BEST_MATCH_RE = re.compile(
    r"Best match at frame # (\d+) \(([\d.]+)s\) with confidence of ([\d.]+)%"
)


def find_vfm() -> str:
    candidates = [
        Path(__file__).parent.parent / "build" / "src" / "vfm",
        Path(__file__).parent.parent / "build" / "vfm",
    ]
    for p in candidates:
        if p.exists():
            return str(p)
    found = shutil.which("vfm")
    if found:
        return found
    return "vfm"


def _bgr_to_pixmap(bgr: np.ndarray) -> QPixmap:
    rgb = cv2.cvtColor(bgr, cv2.COLOR_BGR2RGB)
    h, w, c = rgb.shape
    qimg = QImage(rgb.data, w, h, w * c, QImage.Format.Format_RGB888)
    return QPixmap.fromImage(qimg)


def _fit_pixmap(pixmap: QPixmap, max_w: int, max_h: int) -> QPixmap:
    if pixmap.width() > max_w or pixmap.height() > max_h:
        return pixmap.scaled(
            max_w, max_h,
            Qt.AspectRatioMode.KeepAspectRatio,
            Qt.TransformationMode.SmoothTransformation,
        )
    return pixmap


class FileRow(QWidget):
    def __init__(self, placeholder: str, file_filter: str, parent=None):
        super().__init__(parent)
        self._filter = file_filter

        self.edit = QLineEdit()
        self.edit.setPlaceholderText(placeholder)

        browse = QPushButton("Browse…")
        browse.setFixedWidth(80)
        browse.clicked.connect(self._browse)

        row = QHBoxLayout(self)
        row.setContentsMargins(0, 0, 0, 0)
        row.addWidget(self.edit)
        row.addWidget(browse)

    def _browse(self):
        path, _ = QFileDialog.getOpenFileName(self, "Select file", "", self._filter)
        if path:
            self.edit.setText(path)

    @property
    def path(self) -> str:
        return self.edit.text().strip()


class SaveFileRow(QWidget):
    def __init__(self, placeholder: str, file_filter: str, parent=None):
        super().__init__(parent)
        self._filter = file_filter

        self.edit = QLineEdit()
        self.edit.setPlaceholderText(placeholder)

        browse = QPushButton("Browse…")
        browse.setFixedWidth(80)
        browse.clicked.connect(self._browse)

        row = QHBoxLayout(self)
        row.setContentsMargins(0, 0, 0, 0)
        row.addWidget(self.edit)
        row.addWidget(browse)

    def _browse(self):
        path, _ = QFileDialog.getSaveFileName(self, "Save JSON output", "", self._filter)
        if path:
            self.edit.setText(path)

    @property
    def path(self) -> str:
        return self.edit.text().strip()


class ImagePanel(QWidget):
    """A fixed-size image display with a caption label below."""

    def __init__(self, caption: str, parent=None):
        super().__init__(parent)
        self._image_label = QLabel()
        self._image_label.setFixedSize(PREVIEW_MAX_W, PREVIEW_MAX_H)
        self._image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self._image_label.setStyleSheet("background: #1a1a1a; border: 1px solid #444;")

        self._caption = QLabel(caption)
        self._caption.setAlignment(Qt.AlignmentFlag.AlignCenter)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self._image_label)
        layout.addWidget(self._caption)

    def set_pixmap(self, pixmap: QPixmap):
        self._image_label.setPixmap(_fit_pixmap(pixmap, PREVIEW_MAX_W, PREVIEW_MAX_H))

    def set_caption(self, text: str):
        self._caption.setText(text)

    def clear(self):
        self._image_label.clear()
        self._image_label.setStyleSheet("background: #1a1a1a; border: 1px solid #444;")


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Video Frame Matcher")
        self.setMinimumWidth(860)

        self._process: QProcess | None = None

        # ── inputs ──────────────────────────────────────────────────────────
        inputs_box = QGroupBox("Inputs")
        inputs_form = QFormLayout(inputs_box)

        self.video_row = FileRow(
            "Path to video file…",
            "Video files (*.mp4 *.avi *.mov *.mkv *.webm);;All files (*)",
        )
        self.image_row = FileRow(
            "Path to reference image…",
            "Image files (*.png *.jpg *.jpeg *.bmp *.tiff);;All files (*)",
        )
        inputs_form.addRow("Video:", self.video_row)
        inputs_form.addRow("Image:", self.image_row)

        # ── parameters ──────────────────────────────────────────────────────
        params_box = QGroupBox("Parameters")
        params_form = QFormLayout(params_box)

        self.threshold = QDoubleSpinBox()
        self.threshold.setRange(0.0, 1.0)
        self.threshold.setSingleStep(0.05)
        self.threshold.setDecimals(2)
        self.threshold.setValue(VFM_DEFAULT_THRESHOLD)
        self.threshold.setToolTip("Minimum similarity score (0–1) to count as a match")

        self.frame_step = QSpinBox()
        self.frame_step.setRange(1, 1000)
        self.frame_step.setValue(VFM_DEFAULT_FRAME_STEP)
        self.frame_step.setToolTip("Check every Nth frame")

        self.start_time = QDoubleSpinBox()
        self.start_time.setRange(0.0, 86400.0)
        self.start_time.setSingleStep(1.0)
        self.start_time.setDecimals(2)
        self.start_time.setSuffix(" s")
        self.start_time.setValue(VFM_DEFAULT_START_TIME)

        self.end_time = QDoubleSpinBox()
        self.end_time.setRange(0.0, 86400.0)
        self.end_time.setSingleStep(1.0)
        self.end_time.setDecimals(2)
        self.end_time.setSuffix(" s")
        self.end_time.setSpecialValueText("(end of video)")
        self.end_time.setValue(0.0)
        self.end_time.setToolTip("Leave at 0 to scan until the end of the video")

        self.json_row = SaveFileRow(
            "(optional) output.json",
            "JSON files (*.json);;All files (*)",
        )

        params_form.addRow("Threshold:", self.threshold)
        params_form.addRow("Frame step:", self.frame_step)
        params_form.addRow("Start time:", self.start_time)
        params_form.addRow("End time:", self.end_time)
        params_form.addRow("JSON output:", self.json_row)

        # ── run / stop ───────────────────────────────────────────────────────
        self.run_btn = QPushButton("Run")
        self.run_btn.setFixedHeight(36)
        self.run_btn.clicked.connect(self._run)

        self.stop_btn = QPushButton("Stop")
        self.stop_btn.setFixedHeight(36)
        self.stop_btn.setEnabled(False)
        self.stop_btn.clicked.connect(self._stop)

        btn_row = QHBoxLayout()
        btn_row.addWidget(self.run_btn)
        btn_row.addWidget(self.stop_btn)

        # ── output ───────────────────────────────────────────────────────────
        output_box = QGroupBox("Output")
        output_layout = QVBoxLayout(output_box)

        self.output = QPlainTextEdit()
        self.output.setReadOnly(True)
        self.output.setFont(QFont("Monospace", 10))
        self.output.setMinimumHeight(120)
        self.output.setMaximumHeight(160)
        output_layout.addWidget(self.output)

        # ── match preview ────────────────────────────────────────────────────
        self.preview_box = QGroupBox("Match Preview")
        preview_layout = QHBoxLayout(self.preview_box)
        preview_layout.setSpacing(16)

        self.ref_panel = ImagePanel("Reference Image")
        self.frame_panel = ImagePanel("Best Match Frame")
        preview_layout.addWidget(self.ref_panel)
        preview_layout.addWidget(self.frame_panel)
        self.preview_box.setVisible(False)

        # ── status ───────────────────────────────────────────────────────────
        self.status_label = QLabel("Ready")
        self.status_label.setAlignment(Qt.AlignmentFlag.AlignRight)

        # ── assemble ─────────────────────────────────────────────────────────
        root = QWidget()
        root_layout = QVBoxLayout(root)
        root_layout.addWidget(inputs_box)
        root_layout.addWidget(params_box)
        root_layout.addLayout(btn_row)
        root_layout.addWidget(output_box)
        root_layout.addWidget(self.preview_box)
        root_layout.addWidget(self.status_label)
        self.setCentralWidget(root)

    # ── process management ───────────────────────────────────────────────────

    def _build_args(self) -> list[str]:
        args = [
            "--video", self.video_row.path,
            "--image", self.image_row.path,
            "--threshold", str(self.threshold.value()),
            "--frame-step", str(self.frame_step.value()),
            "--start-time", str(self.start_time.value()),
        ]
        if self.end_time.value() > 0.0:
            args += ["--end-time", str(self.end_time.value())]
        if self.json_row.path:
            args += ["--output-json", self.json_row.path]
        return args

    def _run(self):
        if not self.video_row.path:
            self._append("Error: no video file selected.", error=True)
            return
        if not self.image_row.path:
            self._append("Error: no image file selected.", error=True)
            return

        self.output.clear()
        self.preview_box.setVisible(False)
        self.ref_panel.clear()
        self.frame_panel.clear()
        self.run_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        self.status_label.setText("Running…")

        self._process = QProcess(self)
        self._process.setProcessChannelMode(QProcess.ProcessChannelMode.MergedChannels)
        self._process.readyReadStandardOutput.connect(self._on_output)
        self._process.finished.connect(self._on_finished)

        program = find_vfm()
        args = self._build_args()
        self._append(f"$ {program} {' '.join(args)}\n")
        self._process.start(program, args)

    def _stop(self):
        if self._process and self._process.state() != QProcess.ProcessState.NotRunning:
            self._process.terminate()

    def _on_output(self):
        if self._process:
            text = bytes(self._process.readAllStandardOutput()).decode(errors="replace")
            self._append(text)

    def _on_finished(self, exit_code: int, exit_status: QProcess.ExitStatus):
        self.run_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)

        if exit_status == QProcess.ExitStatus.CrashExit:
            self.status_label.setText("Process crashed")
        elif exit_code == 0:
            self.status_label.setText("Done (exit 0)")
            self._show_match_preview()
        else:
            self.status_label.setText(f"Finished with exit code {exit_code}")

        self._process = None

    # ── match preview ────────────────────────────────────────────────────────

    def _show_match_preview(self):
        text = self.output.toPlainText()
        m = _BEST_MATCH_RE.search(text)
        if not m:
            return

        frame_index = int(m.group(1))
        time_sec = float(m.group(2))
        confidence = float(m.group(3))

        ref_img = cv2.imread(self.image_row.path)
        if ref_img is None:
            self._append("\n[Preview] Could not load reference image.")
            return

        cap = cv2.VideoCapture(self.video_row.path)
        cap.set(cv2.CAP_PROP_POS_FRAMES, frame_index)
        ok, frame = cap.read()
        cap.release()

        if not ok:
            self._append("\n[Preview] Could not extract frame from video.")
            return

        self.ref_panel.set_pixmap(_bgr_to_pixmap(ref_img))
        self.ref_panel.set_caption("Reference Image")

        self.frame_panel.set_pixmap(_bgr_to_pixmap(frame))
        self.frame_panel.set_caption(
            f"Frame #{frame_index}  ·  {time_sec}s  ·  {confidence:.2f}% confidence"
        )

        self.preview_box.setVisible(True)
        self.adjustSize()

    def _append(self, text: str, *, error: bool = False):
        self.output.moveCursor(self.output.textCursor().MoveOperation.End)
        self.output.insertPlainText(text)
        self.output.moveCursor(self.output.textCursor().MoveOperation.End)


def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
