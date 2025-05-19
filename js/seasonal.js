document.addEventListener('DOMContentLoaded', () => {
   console.log("Seasonal script initialized");

   const isWithinChristmasPeriod = () => {
      const today = new Date();
      const currentMonth = today.getMonth(); // 0 = January, 11 = December
      const currentDay = today.getDate();
      return currentMonth === 11 && currentDay >= 1 && currentDay <= 31;
   };

   const updateIcon = () => {
      const iconElement = document.querySelector('.sidebar img[title="icon"]');
      if (!iconElement) return;
      const basePath = '/img/';
      if (localStorage.getItem('christmasThemePreference') === 'enabled' && isWithinChristmasPeriod()) {
         iconElement.src = `${basePath}icon-christmas.png`;
      } else {
         iconElement.src = `${basePath}icon.png`;
      }
   };

   const loadThemeScript = (scriptPath) => {
      const existingScript = document.getElementById('dynamic-theme-script');
      if (existingScript) {
         existingScript.remove();
      }
      const script = document.createElement('script');
      script.id = 'dynamic-theme-script';
      script.src = scriptPath;
      script.async = true;
      document.head.appendChild(script);
   };

   const applyChristmasTheme = () => {
      console.log("Applying Christmas theme");
      document.body.classList.add('christmas-theme');
      updateIcon();
      loadThemeScript('/js/christmas.js');
   };

   const removeChristmasTheme = () => {
      console.log("Removing Christmas theme");
      document.body.classList.remove('christmas-theme');
      updateIcon();
      loadThemeScript('/js/usual.js');
   };

   // Default to "enabled" if no preference is stored
   if (localStorage.getItem('christmasThemePreference') === null) {
      localStorage.setItem('christmasThemePreference', 'enabled');
   }

   const toggleCheckbox = document.getElementById('christmas-theme-toggle');
   const storedThemePreference = localStorage.getItem('christmasThemePreference');

   // Always keep the checkbox checked by default
   if (toggleCheckbox) {
      toggleCheckbox.checked = true;
   }

   // Apply themes based on both stored preference and current date.
   // If it's December and preference is enabled, use the seasonal theme.
   // Otherwise, always load the usual theme.
   if (storedThemePreference === 'enabled' && isWithinChristmasPeriod()) {
      applyChristmasTheme();
   } else {
      removeChristmasTheme();
   }

   if (toggleCheckbox) {
      toggleCheckbox.addEventListener('change', () => {
         if (toggleCheckbox.checked) {
            localStorage.setItem('christmasThemePreference', 'enabled');
            if (isWithinChristmasPeriod()) {
               applyChristmasTheme();
            } else {
               // Even if enabled, only apply seasonal theme in December.
               removeChristmasTheme();
            }
         } else {
            localStorage.setItem('christmasThemePreference', 'disabled');
            removeChristmasTheme();
         }
      });
   }

   updateIcon();
});
