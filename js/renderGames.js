
/**
 * renderGames.js
 *
 * Dynamically renders the games defined in games.js into the #game-container.
 * Applies a fade-in animation with increasing delay from top to bottom.
 * Slower animation version for smoother appearance.
 */

const container = document.getElementById("game-container");

games.forEach((game, index) => {
   const div = document.createElement("div");
   div.className = `column gameDiv ${game.classes}`;
   div.setAttribute("name", game.name);
   div.style.opacity = 0;
   div.style.transform = "translateY(30px)";
   div.style.transition = "opacity 0.8s ease, transform 0.8s ease";
   div.style.transitionDelay = (index * 40) + "ms";

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