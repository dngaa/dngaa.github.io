/*
 * seasonal.js
 * 
 * This script manages the seasonal (Christmas) theme for the website.
 * - By default, the seasonal theme is **enabled** and activates only during December.
 * - Users can opt out by unchecking the "Seasonal Theme" toggle on the settings page.
 * - The user's preference is saved using localStorage.
 * - Applies seasonal styling and dynamically loads related particle effects (e.g., snow).
 * - Updates the site icon based on the current theme and time of year.
 */

document.addEventListener('DOMContentLoaded', () => {
   console.log("Seasonal script initialized");
   
// Function to check if today's date is within the Christmas period (December)
   const isWithinChristmasPeriod = () => {
      const today = new Date();
      const currentMonth = today.getMonth(); // 0 = January, 11 = December
      return currentMonth === 11;
   };

   // Function to update the sidebar icon based on theme and season
   const updateIcon = () => {
      const iconElement = document.querySelector('.sidebar img[title="icon"]');
      if (!iconElement) return;
      const basePath = '/img/';
      const enabled = localStorage.getItem('christmasThemePreference') === 'enabled';
      if (enabled && isWithinChristmasPeriod()) {
         iconElement.src = `${basePath}icon-christmas.png`;
      } else {
         iconElement.src = `${basePath}icon.png`;
      }
   };

   // Function to dynamically load and run the theme (a JS file)
   const loadThemeScript = (scriptPath) => {
      const existingScript = document.getElementById('dynamic-theme-script');
      if (existingScript) existingScript.remove();
      const script = document.createElement('script');
      script.id = 'dynamic-theme-script';
      script.src = scriptPath;
      script.async = true;
      document.head.appendChild(script);
   };

   // loads christmas theme when appropriate
   const applyChristmasTheme = () => {
      console.log("Applying Christmas theme");
      document.body.classList.add('christmas-theme');
      updateIcon();
      loadThemeScript('/js/christmas.js');
   };

   // loads usual theme when appropriate
   const removeChristmasTheme = () => {
      console.log("Removing Christmas theme");
      document.body.classList.remove('christmas-theme');
      updateIcon();
      loadThemeScript('/js/usual.js');
   };

   const toggleCheckbox = document.getElementById('christmas-theme-toggle');

   // Load preference or default to "enabled"
   let preference = localStorage.getItem('christmasThemePreference');
   if (!preference) {
      preference = 'enabled';
      localStorage.setItem('christmasThemePreference', 'enabled');
   }

   // Respect the stored preference
   if (preference === 'enabled' && isWithinChristmasPeriod()) {
      applyChristmasTheme();
   } else {
      removeChristmasTheme();
   }

   // Sync checkbox state with stored preference
   if (toggleCheckbox) {
      toggleCheckbox.checked = preference === 'enabled';

      toggleCheckbox.addEventListener('change', () => {
         const newPref = toggleCheckbox.checked ? 'enabled' : 'disabled';
         localStorage.setItem('christmasThemePreference', newPref);

         if (newPref === 'enabled' && isWithinChristmasPeriod()) {
            applyChristmasTheme();
         } else {
            removeChristmasTheme();
         }
      });
   }

   updateIcon(); // Ensure the icon is updated on load
});
