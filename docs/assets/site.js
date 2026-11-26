const hardwarePage = document.body.dataset.hardwarePage;
const hardwareAnchors = ["#hardware", "#interactive", "#files", "#bom", "#pinout"];

if (hardwarePage && hardwareAnchors.includes(window.location.hash)) {
  window.location.replace(hardwarePage + window.location.hash);
}

const trails = [...document.querySelectorAll("[data-mouse-trail]")];
const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)");
let frame = 0;

function clampProgress(value) {
  return Math.min(1, Math.max(0, value));
}

function moveMice() {
  frame = 0;
  for (const trail of trails) {
    if (reducedMotion.matches) {
      trail.style.removeProperty("--trail-front");
      trail.style.removeProperty("--trail-back");
      continue;
    }

    const { top } = trail.getBoundingClientRect();
    const viewport = window.innerHeight;
    const progress = clampProgress((0.93 * viewport - top) / (0.78 * viewport));
    const headingLeft = trail.dataset.direction === "left";
    const front = headingLeft
      ? 100 - 96 * clampProgress(progress * 1.12)
      : 26 + 74 * clampProgress(progress * 1.22);
    const back = headingLeft
      ? 100 - 79 * clampProgress((progress - 0.13) / 0.87)
      : 4 + 96 * progress;

    trail.style.setProperty("--trail-front", `${front}%`);
    trail.style.setProperty("--trail-back", `${back}%`);
  }
}

function scheduleMice() {
  if (!frame) frame = window.requestAnimationFrame(moveMice);
}

if (trails.length) {
  window.addEventListener("scroll", scheduleMice, { passive: true });
  window.addEventListener("resize", scheduleMice);
  reducedMotion.addEventListener("change", scheduleMice);
  scheduleMice();
}
