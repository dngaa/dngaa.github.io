document.addEventListener('DOMContentLoaded', () => {
    console.log("Seasonal script initialized");

    const toggleCheckbox = document.getElementById('christmas-theme-toggle');
    if (!toggleCheckbox) {
        console.error("Toggle checkbox not found");
        return;
    }

    const today = new Date();
    const currentMonth = today.getMonth(); // 0 = January, 11 = December
    const currentDay = today.getDate(); // Day of the month (1-31)

    // Check if the current date falls between December 1st and January 15th
    const isWithinChristmasPeriod = (currentMonth === 11 && currentDay >= 1) || (currentMonth === 0 && currentDay <= 15);

    const applyChristmasTheme = () => {
        console.log("Applying Christmas theme");
        document.body.classList.add('christmas-theme');
        localStorage.setItem('disableChristmasTheme', 'false');
    };

    const removeChristmasTheme = () => {
        console.log("Removing Christmas theme");
        document.body.classList.remove('christmas-theme');
        localStorage.setItem('disableChristmasTheme', 'true');
    };

    // Retrieve user preference from localStorage
    const userPreference = localStorage.getItem('disableChristmasTheme');
    console.log(`User preference: ${userPreference}, Date: ${today}`);

    // Apply or remove theme based on the date and user preference
    if (isWithinChristmasPeriod && userPreference !== 'true') {
        applyChristmasTheme();
        toggleCheckbox.checked = true;
    } else {
        if (userPreference === 'false') {
            applyChristmasTheme();
            toggleCheckbox.checked = true;
        } else {
            removeChristmasTheme();
            toggleCheckbox.checked = false;
        }
    }

    // Handle manual toggling of the theme
    toggleCheckbox.addEventListener('change', () => {
        if (toggleCheckbox.checked) {
            applyChristmasTheme();
        } else {
            removeChristmasTheme();
        }
    });
});
