async function fetchQuotes() {
    try {
        // Fetch the JSON file
        const response = await fetch('../media/quotes.json'); // Adjust the path as needed
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        const quotes = await response.json(); // Parse JSON
        const randomQuoteObj = quotes[Math.floor(Math.random() * quotes.length)]; // Get a random quote

        // Combine the quote and author
        const fullQuote = `"${randomQuoteObj.quote}" - ${randomQuoteObj.author}`;

        // Type out the quote
        typeEffect(fullQuote, 'random-quote', 50); // 50ms delay between each character
    } catch (error) {
        console.error('Error fetching quotes:', error);
        document.getElementById('random-quote').innerHTML = `"Error loading quote."`;
    }
}

// Typing effect function
function typeEffect(text, elementId, delay) {
    const element = document.getElementById(elementId);
    element.innerHTML = ''; // Clear any previous content
    let i = 0;

    function type() {
        if (i < text.length) {
            element.innerHTML += text.charAt(i); // Add the next character
            i++;
            setTimeout(type, delay); // Delay before typing the next character
        }
    }

    type();
}

// Call the function to fetch and display a random quote
fetchQuotes();
