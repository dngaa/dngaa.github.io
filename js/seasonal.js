document.addEventListener('DOMContentLoaded', () => {
    console.log("Seasonal script initialized");

    // Universal script that works with or without a toggle checkbox
    const toggleCheckbox = document.getElementById('christmas-theme-toggle');

    // Get the current date from the user's device
    const today = new Date();
    const currentMonth = today.getMonth(); // 0 = January, 11 = December
    const currentDay = today.getDate(); // Day of the month (1-31)

    // Precise Christmas period check
    const isWithinChristmasPeriod = () => {
        // Check if it's December 1st to December 31st
        if (currentMonth === 11 && currentDay >= 1 && currentDay <= 31) {
            return true;
        }
        
        // Check if it's January 1st to January 15th
        if (currentMonth === 0 && currentDay >= 1 && currentDay <= 15) {
            return true;
        }
        
        return false;
    };

    const applyChristmasTheme = () => {
        console.log("Applying Christmas theme");
        document.body.classList.add('christmas-theme');
        
        // Dynamically load Christmas script
        const script = document.createElement('script');
        script.type = 'text/javascript';
        
        // Determine script path based on current location
        const currentPath = window.location.pathname;
        script.src = currentPath.includes('/pages/') 
            ? '../js/christmas.js'  // If in pages directory
            : './js/christmas.js';  // If in root directory
        
        script.id = 'dynamic-particles-script';
        
        // Remove existing script if any
        const existingScript = document.getElementById('dynamic-particles-script');
        if (existingScript) {
            existingScript.remove();
        }
        
        document.head.appendChild(script);
    };

    const removeChristmasTheme = () => {
        console.log("Removing Christmas theme");
        document.body.classList.remove('christmas-theme');
        
        // Dynamically load Usual script
        const script = document.createElement('script');
        script.type = 'text/javascript';
        
        // Determine script path based on current location
        const currentPath = window.location.pathname;
        script.src = currentPath.includes('/pages/') 
            ? '../js/usual.js'  // If in pages directory
            : './js/usual.js';  // If in root directory
        
        script.id = 'dynamic-particles-script';
        
        // Remove existing script if any
        const existingScript = document.getElementById('dynamic-particles-script');
        if (existingScript) {
            existingScript.remove();
        }
        
        document.head.appendChild(script);
    };

    // Retrieve user's theme preference
    const storedThemePreference = localStorage.getItem('christmasThemePreference');
    
    // Determine theme application
    if (isWithinChristmasPeriod()) {
        // If it's Christmas time
        if (storedThemePreference === null) {
            // Default to enabled if no previous preference
            applyChristmasTheme();
            
            // Update checkbox if it exists
            if (toggleCheckbox) {
                toggleCheckbox.checked = true;
            }
            
            localStorage.setItem('christmasThemePreference', 'enabled');
        } else if (storedThemePreference === 'enabled') {
            // User previously wanted the theme
            applyChristmasTheme();
            
            // Update checkbox if it exists
            if (toggleCheckbox) {
                toggleCheckbox.checked = true;
            }
        } else {
            // User previously disabled the theme
            removeChristmasTheme();
            
            // Update checkbox if it exists
            if (toggleCheckbox) {
                toggleCheckbox.checked = false;
            }
        }
    } else {
        // Outside Christmas period
        removeChristmasTheme();
        
        // Update checkbox if it exists
        if (toggleCheckbox) {
            toggleCheckbox.checked = storedThemePreference === 'enabled';
        }
    }

    // Add event listener only if toggle checkbox exists
    if (toggleCheckbox) {
        toggleCheckbox.addEventListener('change', () => {
            if (toggleCheckbox.checked) {
                // User wants the theme
                localStorage.setItem('christmasThemePreference', 'enabled');
            } else {
                // User doesn't want the theme
                localStorage.setItem('christmasThemePreference', 'disabled');
            }
            location.reload(); // Reload the page
        });
    }
});
