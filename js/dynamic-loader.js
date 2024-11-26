document.addEventListener("DOMContentLoaded", () => {
    console.log("Dynamic loader initialized");

    // Helper function to determine base path
    const getBasePath = () => {
        const currentPath = window.location.pathname;
        return currentPath.includes("/pages/") ? "../" : "./";
    };

    // Dynamically load scripts
    const loadScript = (filename) => {
        const basePath = getBasePath();
        const script = document.createElement("script");
        script.type = "text/javascript";
        script.src = `${basePath}js/${filename}`;
        document.head.appendChild(script);
    };

    // Dynamically load stylesheets
    const loadStylesheet = (filename) => {
        const basePath = getBasePath();
        const link = document.createElement("link");
        link.rel = "stylesheet";
        link.href = `${basePath}css/${filename}`;
        document.head.appendChild(link);
    };

    // Dynamically update icon
    const updateFavicon = (filename) => {
        const basePath = getBasePath();
        const link = document.querySelector('link[rel="icon"]');
        if (link) {
            link.href = `${basePath}img/${filename}`;
        }
    };

    // Load required assets
    loadScript("particles.js");
    loadScript("seasonal.js");
    loadStylesheet("particles.css");
    updateFavicon("favicon-32x32.png");

    console.log("Assets dynamically loaded");
});
