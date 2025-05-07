(async () => {
   await loadSnowPreset(tsParticles);

   await tsParticles.load({
      id: "tsparticles",
      options: {
         preset: "snow",
      },
   });
})();

initParticles();

console.log('callback - tsparticles.js config loaded');
   