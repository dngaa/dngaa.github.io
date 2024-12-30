document.addEventListener('DOMContentLoaded', () => {
    console.log("Seasonal script initialized");

    const updateIcon = () => {
        const iconElement = document.querySelector('.sidebar img[title="icon"]');
        if (!iconElement) return; // Safeguard in case the icon isn't found

        // Adjust paths dynamically based on the current location
        const basePath = window.location.pathname.includes('/pages/') ? '../img/' : './img/';

        if (isWithinChristmasPeriod() && localStorage.getItem('christmasThemePreference') === 'enabled') {
            iconElement.src = `${basePath}icon-christmas.png`;
        } else {
            iconElement.src = `${basePath}icon.png`;
        }
    };

    const toggleCheckbox = document.getElementById('christmas-theme-toggle');

    const today = new Date();
    const currentMonth = today.getMonth(); // 0 = January, 11 = December
    const currentDay = today.getDate();

    const isWithinChristmasPeriod = () => {
        if (currentMonth === 11 && currentDay >= 1 && currentDay <= 31) {
            return true;
        }
        if (currentMonth === 11 && currentDay >= 1 && currentDay <= 0) {
            return true;
        }
        return false;
    };

    const applyChristmasTheme = () => {
        console.log("Applying Christmas theme");
        document.body.classList.add('christmas-theme');
        updateIcon();

        const script = document.createElement('script');
        script.type = 'text/javascript';
        const currentPath = window.location.pathname;
        script.src = currentPath.includes('/pages/') 
            ? '../js/christmas.js' 
            : './js/christmas.js';
        script.id = 'dynamic-particles-script';

        const existingScript = document.getElementById('dynamic-particles-script');
        if (existingScript) {
            existingScript.remove();
        }
        document.head.appendChild(script);
    };

    const removeChristmasTheme = () => {
        console.log("Removing Christmas theme");
        document.body.classList.remove('christmas-theme');
        updateIcon();

        const script = document.createElement('script');
        script.type = 'text/javascript';
        const currentPath = window.location.pathname;
        script.src = currentPath.includes('/pages/') 
            ? '../js/usual.js' 
            : './js/usual.js';
        script.id = 'dynamic-particles-script';

        const existingScript = document.getElementById('dynamic-particles-script');
        if (existingScript) {
            existingScript.remove();
        }
        document.head.appendChild(script);
    };

    const storedThemePreference = localStorage.getItem('christmasThemePreference');

    if (isWithinChristmasPeriod()) {
        if (storedThemePreference === null) {
            applyChristmasTheme();
            if (toggleCheckbox) toggleCheckbox.checked = true;
            localStorage.setItem('christmasThemePreference', 'enabled');
        } else if (storedThemePreference === 'enabled') {
            applyChristmasTheme();
            if (toggleCheckbox) toggleCheckbox.checked = true;
        } else {
            removeChristmasTheme();
            if (toggleCheckbox) toggleCheckbox.checked = false;
        }
    } else {
        removeChristmasTheme();
        if (toggleCheckbox) {
            toggleCheckbox.checked = storedThemePreference === 'enabled';
        }
    }

    if (toggleCheckbox) {
        toggleCheckbox.addEventListener('change', () => {
            if (toggleCheckbox.checked) {
                localStorage.setItem('christmasThemePreference', 'enabled');
            } else {
                localStorage.setItem('christmasThemePreference', 'disabled');
            }
            location.reload();
        });
    }

    updateIcon(); // Ensure the icon is updated on load
});
