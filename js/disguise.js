document.addEventListener("DOMContentLoaded", () => {
   const checkbox = document.getElementById("gc-disguise-toggle");

   // Apply disguise if previously set to true
   if (localStorage.getItem("gcDisguise") === "true") {
      document.title = "Google Classroom";
      if (checkbox) checkbox.checked = true;
   }

   // Only add listener if checkbox exists (to prevent errors on pages without it)
   if (checkbox) {
      checkbox.addEventListener("change", () => {
         if (checkbox.checked) {
            localStorage.setItem("gcDisguise", "true");
            document.title = "Google Classroom";
         } else {
            localStorage.setItem("gcDisguise", "false");
            document.title = "Jet Yeh's"; // or your default page title
         }
      });
   } else {
      // Still apply the disguise on pages that don't have the checkbox but use this script
      if (localStorage.getItem("gcDisguise") === "true") {
         document.title = "Google Classroom";
      }
   }
});
