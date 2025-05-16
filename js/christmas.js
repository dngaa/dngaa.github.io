(async () => {
   await loadFull(tsParticles);

   await tsParticles.load({
      id: "tsparticles",
      options: {
         background: {
            color: "#00873E"
         },
         particles: {
            number: {
               value: 150,
               density: {
                  enable: true,
                  area: 1000
               }
            },
            color: {
               value: "#fff"
            },
            shape: {
               type: "circle",
               options: {
                  image: {
                     src: "img/github.svg",
                     width: 100,
                     height: 100
                  },
                  polygon: {
                     sides: 5
                  }
               }
            },
            opacity: {
               value: 0.5,
               random: false,
               animation: {
                  enable: true,
                  speed: 0.5,
                  minimumValue: 0.016,
                  sync: false
               }
            },
            size: {
               value: 5,
               random: {
                  enable: true,
                  minimumValue: 0.1
               }
            },
            move: {
               enable: true,
               speed: 0.5,
               direction: "bottom",
               random: true
            }
         },
         interactivity: {
            events: {
               onHover: {
                  enable: false,
                  mode: "grab"
               },
               onClick: {
                  enable: false,
                  mode: "push"
               },
               resize: true
            },
            modes: {
               grab: {
                  distance: 400,
                  links: {
                     opacity: 1
                  }
               },
               bubble: {
                  distance: 400,
                  size: 40,
                  duration: 2,
                  opacity: 8,
                  speed: 3
               }
            }
         }
      }
   });
})();

console.log('callback - tsparticles.js config loaded');