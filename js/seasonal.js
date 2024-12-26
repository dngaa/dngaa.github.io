document.addEventListener('DOMContentLoaded', () => {
    console.log("Seasonal script initialized");

    const toggleCheckbox = document.getElementById('christmas-theme-toggle');
    const today = new Date();
    const currentMonth = today.getMonth();
    const currentDay = today.getDate();

    // Determine if it's within the Christmas period
    const isWithinChristmasPeriod = () => {
        return (currentMonth === 11 && currentDay >= 1 && currentDay <= 31) ||
               (currentMonth === 0 && currentDay >= 1 && currentDay <= 15);
    };

    // Update the icon dynamically
    const updateIcon = (isChristmas) => {
        const iconElement = document.querySelector('.left-col img[title="icon"]');
        if (iconElement) {
            const currentPath = window.location.pathname;
            const basePath = currentPath.includes('/pages/') ? '../img/' : './img/';
            iconElement.src = isChristmas ? `${basePath}icon-christmas.png` : `${basePath}icon.png`;
        }
    };

    // Dynamically load a script
    const loadScript = (scriptName) => {
        const script = document.createElement('script');
        script.type = 'text/javascript';
        const currentPath = window.location.pathname;
        const basePath = currentPath.includes('/pages/') ? '../js/' : './js/';
        script.src = `${basePath}${scriptName}`;
        script.id = 'dynamic-particles-script';

        // Remove existing script if any
        const existingScript = document.getElementById('dynamic-particles-script');
        if (existingScript) {
            existingScript.remove();
        }

        document.head.appendChild(script);
    };

    const applyChristmasTheme = () => {
        console.log("Applying Christmas theme");
        document.body.classList.add('christmas-theme');
        updateIcon(true);
        loadScript('christmas.js');
    };

    const removeChristmasTheme = () => {
        console.log("Removing Christmas theme");
        document.body.classList.remove('christmas-theme');
        updateIcon(false);
        loadScript('usual.js');
    };

    const storedThemePreference = localStorage.getItem('christmasThemePreference');

    // Determine whether to apply or remove the Christmas theme
    if (isWithinChristmasPeriod()) {
        if (storedThemePreference === null || storedThemePreference === 'enabled') {
            applyChristmasTheme();
            if (toggleCheckbox) toggleCheckbox.checked = true;
            localStorage.setItem('christmasThemePreference', 'enabled');
        } else {
            removeChristmasTheme();
            if (toggleCheckbox) toggleCheckbox.checked = false;
        }
    } else {
        removeChristmasTheme();
        if (toggleCheckbox) toggleCheckbox.checked = storedThemePreference === 'enabled';
    }

    // Add toggle event listener
    if (toggleCheckbox) {
        toggleCheckbox.addEventListener('change', () => {
            if (toggleCheckbox.checked) {
                localStorage.setItem('christmasThemePreference', 'enabled');
                applyChristmasTheme();
            } else {
                localStorage.setItem('christmasThemePreference', 'disabled');
                removeChristmasTheme();
            }
            location.reload(); // Reload the page to reflect changes
        });
    }
});
