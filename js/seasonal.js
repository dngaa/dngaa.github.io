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
      return currentMonth === 11 && currentDay >= 1 && currentDay <= 31; // Valid between Dec 1-31
   };

   // Function to dynamically load and run the theme (a JS file)
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

      // Load `christmas.js` for Christmas particle effects
      const basePath = window.location.pathname.includes('/pages/') ? '../js/' : './js/';
      loadThemeScript(`${basePath}christmas.js`);
   };

   const removeChristmasTheme = () => {
      console.log("Removing Christmas theme");
      document.body.classList.remove('christmas-theme');
      updateIcon();

      // Load `usual.js` for usual particle effects
      const basePath = window.location.pathname.includes('/pages/') ? '../js/' : './js/';
      loadThemeScript(`${basePath}usual.js`);
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
            applyChristmasTheme();
         } else {
            localStorage.setItem('christmasThemePreference', 'disabled');
            removeChristmasTheme();
         }
      });
   }

   updateIcon(); // Ensure the icon is updated on load
});