/*
 * renderGames.js
 *
 * Dynamically renders the games defined in games.js into the #game-container.
 * Applies a fade-in animation with increasing delay from top to bottom.
 * Renders first 8 with 220ms delay, rest with 60ms delay.
 */

// Inject disguise.js for all pages that include renderGames.js (i.e., game pages)
const disguiseScript = document.createElement("script");
disguiseScript.src = "/js/disguise.js";  // path from /games/ to /js/
document.head.appendChild(disguiseScript);

const container = document.getElementById("game-container");

games.forEach((game, index) => {
   const div = document.createElement("div");
   div.className = `column gameDiv ${game.classes}`;
   div.setAttribute("name", game.name);
   div.style.opacity = 0;
   div.style.transform = "translateY(30px)";
   div.style.transition = "opacity 0.8s ease, transform 0.8s ease";

   const delay = index < 8 ? index * 220 : (8 * 220) + ((index - 8) * 60);
   div.style.transitionDelay = delay + "ms";

   div.innerHTML = `
      <a class="blogLink" href="${game.link}">
         <div class="content">
            <img loading="lazy" src="${game.img}" alt="${game.name}" style="width:100%">
            <h4>${game.name}</h4>
         </div>
      </a>
   `;
   container.appendChild(div);
});

// Trigger the fade-in animation after rendering
requestAnimationFrame(() => {
   const allGames = document.querySelectorAll(".gameDiv");
   allGames.forEach((el) => {
      el.style.opacity = 1;
      el.style.transform = "translateY(0)";
   });
});
