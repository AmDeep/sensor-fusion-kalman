"""Compare gyro-only and complementary-filter angle estimates."""
from __future__ import annotations

import argparse


def run(alpha: float, samples: int, gyro_rate: float, accel_angle: float) -> tuple[float, float]:
    gyro_only = filtered = 0.0
    for _ in range(samples):
        gyro_only += gyro_rate * 0.01
        filtered = alpha * (filtered + gyro_rate * 0.01) + (1.0 - alpha) * accel_angle
    return gyro_only, filtered


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--alpha", type=float, default=0.98)
    parser.add_argument("--samples", type=int, default=500)
    parser.add_argument("--gyro-rate", type=float, default=0.2)
    parser.add_argument("--accel-angle", type=float, default=0.0)
    args = parser.parse_args()
    raw, fused = run(args.alpha, args.samples, args.gyro_rate, args.accel_angle)
    print(f"gyro_only_deg={raw:.3f} complementary_deg={fused:.3f}")
