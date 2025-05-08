(async () => {
   await loadFull(tsParticles);
   
   await tsParticles.load({
      id: "tsparticles",
      options: {
         particles: {
            number: {
               value: 100,
            },
            size: {
               value: 1,
            },
            links: {
               enable: true,
               distance: 115,
               opacity: 0.25,
            },
            move: {
               enable: true,
               speed: 1.75,
               outModes: {
                  default: "bounce",
               },
            },
         },
      },
   });
})();

console.log('callback - tsparticles.js config loaded');