files <- c(
  "binomial_samples_0_4_a.txt",
  "binomial_samples_0_4_b.txt",
  "binomial_samples_0_4_v.txt",
  "binomial_samples_0_4_g.txt"
)

titles <- c(
  "a: n = 4, p = 0.18",
  "b: n = 4, p = 0.82",
  "v: n = 4, p = 0.5",
  "g: n = 4, p = 0.6"
)

output_path <- "binomial_histogram_0_4_panels.png"
break_points <- seq(-0.5, 4.5, by = 1)
label_points <- 0:4

png(filename = output_path, width = 1600, height = 1200, res = 150)
par(mfrow = c(2, 2), mar = c(6, 4.5, 4, 2))

for (i in seq_along(files)) {
  samples <- scan(files[i], what = integer(), quiet = TRUE)
  hist(
    samples,
    breaks = break_points,
    col = "palegreen3",
    border = "darkgreen",
    main = titles[i],
    xlab = "Сгенерированное значение",
    ylab = "Частота",
    xaxt = "n"
  )
  axis(1, at = break_points, labels = FALSE, tck = -0.02)
  axis(1, at = label_points, labels = label_points, tick = FALSE, line = 0.8)
  grid(nx = NA, ny = NULL, col = "gray80", lty = "dotted")
}

dev.off()

cat(sprintf("Histogram saved to %s\n", output_path))
