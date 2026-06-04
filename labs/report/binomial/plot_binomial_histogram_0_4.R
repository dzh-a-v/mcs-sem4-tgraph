# Report figure: histogram of 1000 simulated values B(4, 0.6)
input_path <- "binomial_samples_0_4_1000.txt"
output_path <- "binomial_histogram_0_4.png"

samples <- scan(input_path, what = integer(), quiet = TRUE)
break_points <- seq(-0.5, 4.5, by = 1)
label_points <- 0:4

png(filename = output_path, width = 900, height = 700, res = 150)
hist(
  samples,
  breaks = break_points,
  col = "palegreen3",
  border = "darkgreen",
  main = "n = 4, p = 0.6",
  xlab = "Сгенерированное значение",
  ylab = "Частота",
  xaxt = "n"
)
axis(1, at = break_points, labels = FALSE, tck = -0.02)
axis(1, at = label_points, labels = label_points, tick = FALSE, line = 0.8)
grid(nx = NA, ny = NULL, col = "gray80", lty = "dotted")
dev.off()

cat(sprintf("Histogram saved to %s\n", output_path))
