document.addEventListener('DOMContentLoaded', () => {
   console.log("Seasonal script initialized");

   const isWithinChristmasPeriod = () => {
      const today = new Date();
      const currentMonth = today.getMonth();
      const currentDay = today.getDate();
      return currentMonth === 11 && currentDay >= 1 && currentDay <= 31;
   };

   const updateIcon = () => {
      const iconElement = document.querySelector('.sidebar img[title="icon"]');
      if (!iconElement) return;
      const basePath = window.location.pathname.includes('/pages/') ? '../img/' : './img/';
      if (localStorage.getItem('christmasThemePreference') === 'enabled') {
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
      const basePath = window.location.pathname.includes('/pages/') ? '../js/' : './js/';
      loadThemeScript(`${basePath}christmas.js`);
   };

   const removeChristmasTheme = () => {
      console.log("Removing Christmas theme");
      document.body.classList.remove('christmas-theme');
      updateIcon();
      const basePath = window.location.pathname.includes('/pages/') ? '../js/' : './js/';
      loadThemeScript(`${basePath}usual.js`);
   };

   const toggleCheckbox = document.getElementById('christmas-theme-toggle');
   const storedThemePreference = localStorage.getItem('christmasThemePreference');

   if (storedThemePreference === 'enabled') {
      applyChristmasTheme();
      if (toggleCheckbox) toggleCheckbox.checked = true;
   } else if (storedThemePreference === 'disabled') {
      removeChristmasTheme();
      if (toggleCheckbox) toggleCheckbox.checked = false;
   } else if (isWithinChristmasPeriod()) {
      applyChristmasTheme();
      if (toggleCheckbox) toggleCheckbox.checked = true;
      localStorage.setItem('christmasThemePreference', 'enabled');
   } else {
      removeChristmasTheme();
      if (toggleCheckbox) toggleCheckbox.checked = false;
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

   updateIcon();
});
