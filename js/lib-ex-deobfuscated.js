(function () {
  // Function to log the domain and referrer, and display a message if conditions are met
  function check() {
    console.log("Checking this page... Please wait.");
    var domain = document.domain;
    console.log("Domain: " + domain);

    if (domain === "3kh0.github.io") {
      console.log("This page is on the 3kh0.github.io domain!");
      var referrer = document.referrer;
      console.log("Referrer: " + referrer);

      if (referrer !== '') {
        document.write("<h1>Hello gamers! Please go <a href='https://3kh0.github.io/'>here</a> for better games.</h1><h3>Site-owners, if you are confused on why this happened, please read <a href='https://github.com/3kh0/3kh0.github.io/wiki/Note-to-people-stealing-my-games'>this</a>.</h3>");
        console.log("Sorry, this page was embedded, and that is not allowed!");
      }
    } else {
      document.write("<h1>Hello gamers! Please go <a href='https://3kh0.github.io/'>here</a> for better games.</h1><h3>Site-owners, if you are confused on why this happened, please read <a href='https://github.com/3kh0/3kh0.github.io/wiki/Note-to-people-stealing-my-games'>this</a>.</h3>");
      console.log("Sorry, but this code is copyrighted so you cannot just yoink it! Want to have this game on your site? Read this: https://github.com/3kh0/3kh0.github.io/wiki/Note-to-people-stealing-my-games");
    }
    console.log("The check is complete! You can use check(); to run it again if you wish.");
  }

  // Function to write a message to the document
  function msg() {
    var message = "<h1>Hello gamers! Please go <a href='https://3kh0.github.io/'>here</a> for better games.</h1><h3>Site-owners, if you are confused on why this happened, please read <a href='https://github.com/3kh0/3kh0.github.io/wiki/Note-to-people-stealing-my-games'>this</a>.</h3>";
    document.write(message);
    console.log("Done! :)");
  }

  // Function to log the referrer
  function getReferrer() {
    var referrer = document.referrer;
    console.log("Referrer: " + referrer);
  }

  // Function to log the domain
  function getDomain() {
    var domain = document.domain;
    console.log("Domain: " + domain);
  }

  // Dummy function used in the original script
  function dummyFunction() {
    // This function does nothing in the deobfuscated version
  }

  // Repeatedly call the dummy function every 4 seconds
  setInterval(dummyFunction, 4000);

  // Immediately invoke the check function
  check();
})();
