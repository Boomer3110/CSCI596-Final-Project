"""Generate the CPU benchmark figure using ReportLab."""
import csv
from pathlib import Path

from reportlab.graphics import renderSVG
from reportlab.graphics.charts.lineplots import LinePlot
from reportlab.graphics.shapes import Drawing, String
from reportlab.lib.colors import HexColor

root = Path(__file__).parent
with (root / "benchmark.csv").open() as handle:
    rows = list(csv.DictReader(handle))

drawing = Drawing(680, 290)
for offset, title, columns in [
    (0, "Milliseconds per step", ["all_pairs_ms", "grid_ms"]),
    (340, "CPU speedup (times faster)", ["speedup"]),
]:
    chart = LinePlot()
    chart.x, chart.y, chart.width, chart.height = offset + 42, 54, 267, 168
    chart.data = [[(int(row["particles"]), float(row[col])) for row in rows] for col in columns]
    chart.xValueAxis.valueMin = chart.yValueAxis.valueMin = 0
    chart.xValueAxis.valueSteps = [0, 1000, 2000, 3000]
    chart.lines[0].strokeColor = HexColor("#255487")
    chart.lines[1].strokeColor = HexColor("#b6461b")
    drawing.add(chart)
    drawing.add(String(offset + 42, 253, title, fontName="Helvetica-Bold", fontSize=15))
    drawing.add(String(offset + 145, 20, "Particles"))
drawing.add(String(42, 232, "All-pairs", fillColor=HexColor("#255487")))
drawing.add(String(110, 232, "Uniform grid", fillColor=HexColor("#b6461b")))
renderSVG.drawToFile(drawing, str(root / "benchmark.svg"))
