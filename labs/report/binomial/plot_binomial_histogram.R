input_path <- "binomial_samples_1000.txt"
output_path <- "binomial_histogram.png"

samples <- scan(input_path, what = integer(), quiet = TRUE)
break_points <- seq(min(samples) - 0.5, max(samples) + 0.5, by = 1)
label_points <- min(samples):max(samples)

png(filename = output_path, width = 1400, height = 900, res = 150)
par(mar = c(6, 4.5, 4.5, 2))
hist(
  samples,
  breaks = break_points,
  col = "skyblue",
  border = "steelblue4",
  main = "Гистограмма 1000 значений биномиального распределения",
  xlab = "Сгенерированное значение",
  ylab = "Частота",
  xaxt = "n"
)
axis(1, at = break_points, labels = FALSE, tck = -0.02)
axis(1, at = label_points, labels = label_points, tick = FALSE, line = 0.8)
grid(nx = NA, ny = NULL, col = "gray80", lty = "dotted")
dev.off()

cat(sprintf("Histogram saved to %s\n", output_path))
