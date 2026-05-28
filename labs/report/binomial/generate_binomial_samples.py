from pathlib import Path
import random


N = 10
P = 0.5
SAMPLE_COUNT = 1000
SEED = 42
OUTPUT_PATH = Path(__file__).with_name("binomial_samples_1000.txt")


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
    rng = random.Random(SEED)
    samples = []
    for _ in range(SAMPLE_COUNT):
        value = 0
        while value == 0:
            value = sample_binomial(N, P, rng)
        samples.append(value)

    OUTPUT_PATH.write_text(
        "\n".join(str(value) for value in samples) + "\n",
        encoding="utf-8",
    )

    print(f"Saved {len(samples)} samples to {OUTPUT_PATH}")


if __name__ == "__main__":
    main()
