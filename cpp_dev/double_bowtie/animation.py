import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# ---- Load boundary & gates ----
with open("walk.txt") as f:
    # Boundary
    x_axis_vals_boundary = np.fromstring(f.readline(), sep=" ")
    y_axis_vals_boundary = np.fromstring(f.readline(), sep=" ")

    # Gate 1
    gate1X = np.fromstring(f.readline(), sep=" ")
    gate1Y = np.fromstring(f.readline(), sep=" ")

    # Box
    boxX = np.fromstring(f.readline(), sep=" ")
    boxY = np.fromstring(f.readline(), sep=" ")

    # Gate 3
    gate3X = np.fromstring(f.readline(), sep=" ")
    gate3Y = np.fromstring(f.readline(), sep=" ")

# ---- Load walk data ----
data = np.loadtxt("walk.txt", skiprows=8)

# Columns from C++ output
xdata = data[:,0]
ydata = data[:,1]
walk_count = data[:,2].astype(int)   # total walk steps
gateCrossedCount1 = data[:,3].astype(int)
finalCrossGate1 = data[:,4].astype(int)
gateCrossedCount2 = data[:,5].astype(int)
finalCrossGate2 = data[:,6].astype(int)
gateCrossedCount3 = data[:,7].astype(int)
finalCrossGate3 = data[:,8].astype(int)

n_steps = len(xdata)

# ---- Setup figure ----
fig, ax = plt.subplots(figsize=(6,6))
ax.set_xlim(min(x_axis_vals_boundary)-5, max(x_axis_vals_boundary)+5)
ax.set_ylim(min(y_axis_vals_boundary)-5, max(y_axis_vals_boundary)+5)

# Draw boundaries/gates
ax.plot(x_axis_vals_boundary, y_axis_vals_boundary, 'k--', lw=2, label="Boundary")
ax.plot(gate1X, gate1Y, 'r-', lw=2, label="Gate1")
ax.plot(boxX, boxY, 'g-', lw=2, label="Box")
ax.plot(gate3X, gate3Y, 'b-', lw=2, label="Gate3")

# Path and current position
line, = ax.plot([], [], 'm-', lw=2)
dot, = ax.plot([], [], 'mo')

# Dynamic text
text = ax.text(0.02, 0.95, '', transform=ax.transAxes, fontsize=10, va="top")

ax.legend(loc="upper right")

# ---- Init ----
def init():
    line.set_data([], [])
    dot.set_data([], [])
    text.set_text('')
    return line, dot, text

# ---- Update ----
def update(frame):
    line.set_data(xdata[:frame], ydata[:frame])
    dot.set_data([xdata[frame-1]], [ydata[frame-1]])

    # Update text (format as you like)
    text.set_text(
        f"Step: {frame}/{walk_count[-1]}\n"
        f"Gate1 crossed: {gateCrossedCount1[frame-1]} (final {finalCrossGate1[frame-1]})\n"
        f"Gate2 crossed: {gateCrossedCount2[frame-1]} (final {finalCrossGate2[frame-1]})\n"
        f"Gate3 crossed: {gateCrossedCount3[frame-1]} (final {finalCrossGate3[frame-1]})"
    )

    return line, dot, text

# ---- Animate ----
ani = FuncAnimation(fig, update, frames=n_steps,
                    init_func=init, interval=1, blit=True)

plt.show()
