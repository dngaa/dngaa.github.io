This is a bookmarklet to hide the browsing of a site by piggybacking on an existing browser tab.

Basically, the idea is to be able to browse a site without revealing its browser tab, as well as be able to hide the site view when needed, in case someone else is near, without closing the browsing session of that site.

Keep in mind that this trick uses IFRAME as a sub browser (in other site's web page). So, not all sites allow themselves to be served/displayed in an IFRAME, and some sites may aware that they're being served in an IFRAME where they will "get out" of the IFRAME and replace the browser tab's web page. Sites like those won't work with this bookmarklet.

Additional notes:

If the site is not yet opened (hidden or not), the bookmarklet will open the site.

If the site is visible, the bookmarklet will show a prompt to hide the site view, or close the site (and end the browsing session for that site).

If the site and the prompt is visible, the bookmarklet will hide both.

When the prompt is visible, clicking on the dimmed area will close the prompt and resume browsing the hidden site.

If the site is already opened but is hidden, the bookmarklet will unhide the site view.

When the site is already open but is hidden, a small yellow quare marker at botton-right of browser tab's view will appear to indicate that a site is already opened. In case one might forgot to close the site browsing session.

If a bookmarlet for site A is already open bookmarklets for site B, C, etc. will work like bookmarlet for site A. i.e. they will only work for hiding/showing the site view or prompt. The site must be closed in order to open other hidden site in the same browser tab.

If a hidden site is already open and is visible, switching to other tab or other application, will automatically hide the site view.
