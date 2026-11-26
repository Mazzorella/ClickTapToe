const viewer = document.querySelector("#photo-viewer");
const viewerFigure = viewer.querySelector("figure");
const viewerCaption = viewerFigure.querySelector("figcaption");
const viewerImage = document.createElement("img");
viewerFigure.prepend(viewerImage);

viewer.addEventListener("click", (event) => {
  if (event.target === viewer) viewer.close();
});

function photoButton(className, label) {
  const button = document.createElement("button");
  button.type = "button";
  button.className = `photo-button ${className}`;
  button.setAttribute("aria-label", label);
  return button;
}

function displayPhoto(image, caption, photo) {
  image.src = photo.src;
  image.alt = photo.alt;
  image.width = photo.width;
  image.height = photo.height;
  caption.textContent = photo.caption;
}

for (const gallery of document.querySelectorAll("[data-gallery]")) {
  const figures = [...gallery.querySelectorAll("figure")];
  if (figures.length === 0) continue;

  const photos = figures.map((figure) => {
    const { src, alt, width, height } = figure.querySelector("img");
    return { src, alt, width, height, caption: figure.querySelector("figcaption").textContent };
  });

  const stage = figures[0];
  const stageImage = stage.querySelector("img");
  const stageCaption = stage.querySelector("figcaption");
  const frame = document.createElement("div");
  frame.className = "gallery-image";
  stage.prepend(frame);
  frame.append(stageImage);

  let active = 0;

  const expand = photoButton("gallery-expand", "Expand photo");
  expand.setAttribute("aria-haspopup", "dialog");
  expand.title = "Expand photo";
  expand.addEventListener("click", () => {
    displayPhoto(viewerImage, viewerCaption, photos[active]);
    viewerImage.classList.toggle("slipper-crop", stageImage.classList.contains("slipper-crop"));
    expand.focus({ preventScroll: true });
    viewer.showModal();
  });
  frame.append(expand);

  if (photos.length === 1) continue;

  const previous = photoButton("gallery-arrow gallery-previous", "Previous photo");
  const next = photoButton("gallery-arrow gallery-next", "Next photo");
  frame.append(previous, next);

  const status = document.createElement("p");
  status.className = "gallery-status";
  status.setAttribute("aria-live", "polite");

  const track = document.createElement("div");
  track.className = "gallery-thumbnails";
  track.setAttribute("role", "group");
  track.setAttribute("aria-label", "Choose a photo");

  const thumbnails = photos.map((photo, index) => {
    const image = document.createElement("img");
    image.src = photo.src;
    image.alt = "";
    image.width = photo.width;
    image.height = photo.height;
    image.loading = "lazy";
    const button = document.createElement("button");
    button.type = "button";
    button.className = "gallery-thumbnail";
    button.tabIndex = -1;
    button.setAttribute("aria-label", `Photo ${index + 1}, ${photo.caption}`);
    button.append(image);
    button.addEventListener("click", () => showPhoto(index));
    track.append(button);
    return button;
  });

  function showPhoto(index) {
    thumbnails[active].tabIndex = -1;
    thumbnails[active].removeAttribute("aria-current");

    active = (index + photos.length) % photos.length;
    const button = thumbnails[active];
    displayPhoto(stageImage, stageCaption, photos[active]);
    button.tabIndex = 0;
    button.setAttribute("aria-current", "true");
    status.textContent = `Photo ${active + 1} of ${photos.length}`;

    const bounds = track.getBoundingClientRect();
    const selected = button.getBoundingClientRect();
    if (selected.left < bounds.left) {
      track.scrollLeft += selected.left - bounds.left - 4;
    } else if (selected.right > bounds.right) {
      track.scrollLeft += selected.right - bounds.right + 4;
    }
  }

  previous.addEventListener("click", () => showPhoto(active - 1));
  next.addEventListener("click", () => showPhoto(active + 1));
  track.addEventListener("keydown", (event) => {
    if (event.key !== "ArrowLeft" && event.key !== "ArrowRight") return;
    event.preventDefault();
    showPhoto(active + (event.key === "ArrowLeft" ? -1 : 1));
    thumbnails[active].focus({ preventScroll: true });
  });

  gallery.replaceChildren(stage, track, status);
  showPhoto(0);
}
