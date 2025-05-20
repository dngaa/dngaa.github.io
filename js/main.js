/*
 * main.js
 *
 * Handles interactive functionality for the Jet Yeh game website.
 * Includes:
 * - Page loading bar and scroll progress ring
 * - "Go to top" button
 * - Game filtering by category
 * - Game search bar functionality
 * - Music play/pause toggle
 * - Random quote fetch and typewriter effect
 */

document.addEventListener("DOMContentLoaded", () => {
   // Loading bar
   const loadingBar = document.getElementById("loading-bar");
   loadingBar.style.width = "100%";
   setTimeout(() => {
      loadingBar.style.opacity = 0;
      setTimeout(() => (loadingBar.style.display = "none"), 500);
   }, 500);

   // Go to top
   const goToTopButton = document.getElementById("go-to-top-button");
   if (goToTopButton) {
      goToTopButton.addEventListener("click", () => {
         window.scroll({ top: 0, behavior: "smooth" });
      });
   }

   // Scroll percentage bar
   window.addEventListener("scroll", () => {
      const progress = document.getElementById("progress");
      const value = document.getElementById("progress-value");
      const pos = document.documentElement.scrollTop;
      const calcHeight = document.documentElement.scrollHeight - document.documentElement.clientHeight;
      const scrollValue = Math.round((pos * 100) / calcHeight);
      progress.style.background = `conic-gradient(#A61414 ${scrollValue}%, #3D3D3D ${scrollValue}%)`;
   });

   window.dispatchEvent(new Event('scroll'));

   // Game filtering
   filterSelection("all");

   const buttons = document.getElementsByClassName('selectable');
   const games = document.getElementsByClassName('gameDiv');

   function filterSelection(c) {
      const x = document.getElementsByClassName("column");
      const category = c === "all" ? "" : c;
      for (let i = 0; i < x.length; i++) {
         w3RemoveClass(x[i], "show");
         if (x[i].className.indexOf(category) > -1) w3AddClass(x[i], "show");
      }
   }

   function w3AddClass(element, name) {
      const arr1 = element.className.split(" ");
      const arr2 = name.split(" ");
      for (let i = 0; i < arr2.length; i++) {
         if (!arr1.includes(arr2[i])) {
            element.className += " " + arr2[i];
         }
      }
   }

   function w3RemoveClass(element, name) {
      let arr1 = element.className.split(" ");
      const arr2 = name.split(" ");
      arr1 = arr1.filter(cls => !arr2.includes(cls));
      element.className = arr1.join(" ");
   }

   let selectedTopic = "all";

   window.switchGame = function (x) {
      selectedTopic = x;
      for (let i = 0; i < buttons.length; i++) {
         const buttonName = buttons[i].getAttribute("name");
         if (selectedTopic === buttonName) {
            buttons[i].classList.add('selectedButton');
            buttons[i].classList.remove('selectButton');
         } else {
            buttons[i].classList.remove('selectedButton');
            buttons[i].classList.add('selectButton');
         }
      }

      for (let i = 0; i < games.length; i++) {
         games[i].style.display = games[i].classList.contains(selectedTopic) ? "" : "none";
      }
   }

   window.searchFunction = function () {
      const input = document.getElementById('myinput');
      const filter = input.value.toUpperCase();
      const li = document.getElementsByClassName('gameDiv');
      for (let i = 0; i < li.length; i++) {
         const gameName = li[i].getAttribute("name").toUpperCase();
         if (gameName.includes(filter) && li[i].classList.contains(selectedTopic)) {
            li[i].style.display = "";
         } else {
            li[i].style.display = "none";
         }
      }
   }

   // Music toggle
   const mySong = document.getElementById("mySong");
   const icon = document.getElementById("icon");

   if (mySong && icon) {
      icon.addEventListener("click", () => {
         if (mySong.paused) {
            mySong.play();
            icon.src = "img/pause.png";
         } else {
            mySong.pause();
            icon.src = "img/play.png";
         }
      });
   }
});

/**
 * fetchQuotes()
 *
 * Fetches a random quote from a local JSON file and types it out with a typewriter effect.
 */
async function fetchQuotes() {
   try {
      const response = await fetch('../media/quotes.json');
      if (!response.ok) {
         throw new Error(`HTTP error! status: ${response.status}`);
      }

      const quotes = await response.json();
      const randomQuoteObj = quotes[Math.floor(Math.random() * quotes.length)];
      const fullQuote = `"${randomQuoteObj.quote}" - ${randomQuoteObj.author}`;
      typeEffect(fullQuote, 'random-quote', 50);
   } catch (error) {
      console.error('Error fetching quotes:', error);
      document.getElementById('random-quote').innerHTML = `"Error loading quote."`;
   }
}

/**
 * typeEffect()
 *
 * Displays text one character at a time inside a DOM element.
 */
function typeEffect(text, elementId, delay) {
   const element = document.getElementById(elementId);
   element.innerHTML = '';
   let i = 0;

   function type() {
      if (i < text.length) {
         element.innerHTML += text.charAt(i);
         i++;
         setTimeout(type, delay);
      }
   }

   type();
}

fetchQuotes();
