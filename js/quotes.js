async function fetchQuotes() {
    try {
        // Fetch the JSON file
        const response = await fetch('../media/quotes.json'); // Adjust the path if necessary
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        // Parse the JSON data
        const quotes = await response.json();

        // Pick a random quote object
        const randomQuoteObj = quotes[Math.floor(Math.random() * quotes.length)];

        // Display the quote with the author in the desired format
        document.getElementById('random-quote').innerHTML = `"${randomQuoteObj.quote}" - ${randomQuoteObj.author}`;
    } catch (error) {
        console.error('Error fetching quotes:', error);
        document.getElementById('random-quote').innerHTML = `"Error loading quote."`;
    }
}

// Call the function to fetch and display a random quote
fetchQuotes();