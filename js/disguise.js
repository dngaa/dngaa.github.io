document.addEventListener("DOMContentLoaded", () => {
   const checkbox = document.getElementById("gc-disguise-toggle");

   const enableDisguise = () => {
      document.title = "Google Classroom";

      let link = document.querySelector("link[rel~='icon']");
      if (!link) {
         link = document.createElement("link");
         link.rel = "icon";
         document.head.appendChild(link);
      }
      link.href = "/img/favicon-gc-16x16.png";
   };

   const disableDisguise = () => {
      document.title = "Jet Yeh's";

      let link = document.querySelector("link[rel~='icon']");
      if (link) {
         link.href = "/img/favicon-32x32.png"; // default icon
      }
   };

   if (localStorage.getItem("gcDisguise") === "true") {
      enableDisguise();
      if (checkbox) checkbox.checked = true;
   }

   if (checkbox) {
      checkbox.addEventListener("change", () => {
         if (checkbox.checked) {
            localStorage.setItem("gcDisguise", "true");
            enableDisguise();
         } else {
            localStorage.setItem("gcDisguise", "false");
            disableDisguise();
         }
      });
   } else {
      // Fallback if disguise is on but no checkbox exists
      if (localStorage.getItem("gcDisguise") === "true") {
         enableDisguise();
      }
   }
});
