document.addEventListener("readystatechange", () => {
    const loadingBar = document.getElementById("loading-bar");
    const progress = (document.readyState === "interactive") ? 50 : (document.readyState === "complete") ? 100 : 0;
    loadingBar.style.width = progress + "%";
  
    if (progress === 100) {
      setTimeout(() => {
        loadingBar.style.opacity = 0;
        setTimeout(() => (loadingBar.style.display = "none"), 500);
      }, 500);
    }
  });
  