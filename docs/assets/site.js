const trails = [...document.querySelectorAll("[data-mouse-trail]")];
const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)");
let frame = 0;

function moveMice() {
  frame = 0;
  for (const trail of trails) {
    if (reducedMotion.matches) {
      trail.style.removeProperty("--trail-front");
      trail.style.removeProperty("--trail-back");
      continue;
    }

    const { top, height } = trail.getBoundingClientRect();
    const progress = Math.min(1, Math.max(0, (window.innerHeight - top) / (window.innerHeight + height)));
    trail.style.setProperty("--trail-front", `${26 + 74 * progress}%`);
    trail.style.setProperty("--trail-back", `${4 + 72 * progress}%`);
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
