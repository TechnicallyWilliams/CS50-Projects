# CS50 Projects

# Week 4
copy - Used File i/o to copy bitmaps pixels to another one at a time and learned how to recognize the type of file by it’s internal as opposed to its file extension

resize - Wrote a program called resize that resizes 24-bit uncompressed BMPs by a factor of n.Used File i/o to resize BMP images one pixel at a time and learned how to recognize bmp images by their internal signature as oppose to its file extension

recover - Used File i/o to recover 50 jpegs images by recognizing it’s internal signature


# Week 6
Implemented the fastest spell-checker possible by reading words from a dictionary file, loading them into memory, spell checking them against a novel in the file, then unloading the dictionary from memory.

Used File i/o to load a linked list into memory from a dictionary and spell check the entire contents of a book against the linked list using binary search. 

    o   Implemented the following functions: - check() - load() - size() - unload() 
    o   Data Structures: - Singly Linked Lists; 
    o   Algorithms: Binary Search

# Week 7
Implemented my own web server that serves static content (i.e., files ending in .html, .jpg, et al.) and dynamic content (i.e., files ending in .php).

    o   Implemented the following functions: - lookup()   - parse()   - query()   - load()   - indexes()
 
# Week 8
Implemented a Web-based tool with which I could manage a portfolios of stocks. Allowing me to check real stocks' actual prices and portfolios' values, and allowing me to buy (okay, "buy") and sell (fine, "sell") stocks! From a technical perspective, this was done by building a PHP AJAX call to Yahoo’s csv file and then performing file I/O to read the stock information.

# Week 9
Implemented an interactive Google Map's page via Google Maps API and used PHP AJAX calls to talk to Google's News RSS feed in order to return a list of Nationwide news articles based on local zip codes. Geographic search suggestions were also used by implementing the typeahead Javascript Library.

         o   Uses Raw Sql when talking directly to web express version of mySql database
         o   Uses parameterized mySql queries to prevent Sql Injection attacks

# Week 10 - 12 (FINAL PROJECT)
 SPA that allows all users across the internet to finally add sounds to their static images and logos using File I/O functions from JavaScript to upload images and sound.
 
         o   Uses Raw Sql in repository classes to reduce dependence on unnatural LINQ syntax
         o   Uses parameterized Sql queries to prevent Sql Injection attacks
         o   Requests direct access of users microphone and camera
         o   Passes 64-bit encoded images/sounds to backend to then decode and store on server
