from pathlib import Path
import random


N = 4
PARAMETERS = [
    ("a", 0.18),
    ("b", 0.82),
    ("v", 0.5),
    ("g", 0.6),
]
SAMPLE_COUNT = 1000
SEED = 42
OUTPUT_DIR = Path(__file__).parent


def sample_binomial(n: int, p: float, rng: random.Random) -> int:
    q = 1.0 - p
    ratio = p / q
    probability = q ** n

    random_val = rng.random()
    x = 0

    while True:
        random_val -= probability
        if random_val < 0.0:
            return x

        x += 1
        probability *= ratio * (n + 1 - x) / x


def main() -> None:
    for index, (name, p) in enumerate(PARAMETERS):
        rng = random.Random(SEED + index)
        samples = [sample_binomial(N, p, rng) for _ in range(SAMPLE_COUNT)]
        output_path = OUTPUT_DIR / f"binomial_samples_0_4_{name}.txt"
        output_path.write_text(
            "\n".join(str(value) for value in samples) + "\n",
            encoding="utf-8",
        )
        print(f"Saved {len(samples)} samples to {output_path}")


if __name__ == "__main__":
    main()
