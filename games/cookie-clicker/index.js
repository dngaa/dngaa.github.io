<script>
info="
<!DOCTYPE html>
<html>
<head>
<script type=&#34;text/javascript&#34;>
    window.cookieconsent_options = {&#34;message&#34;:&#34;Unsurprisingly, this website uses cookies for ads and traffic analysis.&#34;,&#34;dismiss&#34;:&#34;Got it!&#34;,&#34;learnMore&#34;:&#34;Learn more&#34;,&#34;link&#34;:&#34;//orteil.dashnet.org/cookieconsentpolicy.html&#34;,&#34;target&#34;:&#34;_blank&#34;,&#34;theme&#34;:&#34;//orteil.dashnet.org/cookieconsent.css&#34;,&#34;domain&#34;:&#34;dashnet.org&#34;};
</script>

<script type=&#34;text/javascript&#34; src=&#34;//cdnjs.cloudflare.com/ajax/libs/cookieconsent2/1.0.9/cookieconsent.min.js&#34;></script>

<title>Cookie Clicker</title>
<!--
Code and graphics copyright Orteil, 2013-2021
Feel free to alter this code to your liking, but please do not re-host it, do not profit from it and do not present it as your own.
-->

<meta name=&#34;viewport&#34; content=&#34;width=900, initial-scale=1&#34;>
<link rel=&#34;shortcut icon&#34; href=&#34;img/favicon.ico&#34; />
<!--<link href=&#34;https://fonts.googleapis.com/css?family=Kavoon&subset=latin,latin-ext&#34; rel=&#34;stylesheet&#34; type=&#34;text/css&#34;>-->
<link href=&#39;https://fonts.googleapis.com/css?family=Merriweather:900&subset=latin,latin-ext&#39; rel=&#39;stylesheet&#39; type=&#39;text/css&#39;>
<script src=&#34;showads.js&#34;></script><!-- this just detects adblockers so we can adjust the layout and play nice -->
<script src=&#34;base64.js&#34;></script>
<script src=&#34;main.js?v=2.091&#34;></script>
<script async src=&#34;https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js&#34;></script>
<script>
     (adsbygoogle = window.adsbygoogle || []).push({
          google_ad_client: &#34;ca-pub-8491708950677704&#34;,
          enable_page_level_ads: true
     });
</script>
<link href=&#34;style.css?v=2.053&#34; rel=&#34;stylesheet&#34; type=&#34;text/css&#34;>
<!--[if IE]>
<style type=&#34;text/css&#34;>
	#ifIE9{display:block;}
</style>
<![endif]-->

<!--[if lt IE9]><script src=&#34;excanvas.compiled.js&#34;></script><![endif]-->
<!-- Facebook Pixel Code -->
	<script>
	!function(f,b,e,v,n,t,s){if(f.fbq)return;n=f.fbq=function(){n.callMethod?
	n.callMethod.apply(n,arguments):n.queue.push(arguments)};if(!f._fbq)f._fbq=n;
	n.push=n;n.loaded=!0;n.version=&#39;2.0&#39;;n.queue=[];t=b.createElement(e);t.async=!0;
	t.src=v;s=b.getElementsByTagName(e)[0];s.parentNode.insertBefore(t,s)}(window,
	document,&#39;script&#39;,&#39;https://connect.facebook.net/en_US/fbevents.js&#39;);
	fbq(&#39;init&#39;, &#39;357399801355203&#39;);
	fbq(&#39;track&#39;, &#39;PageView&#39;);
	</script>
	<noscript><img height=&#34;1&#34; width=&#34;1&#34; style=&#34;display:none&#34;
	src=&#34;https://www.facebook.com/tr?id=357399801355203&ev=PageView&noscript=1&#34;></noscript>
<!-- end code -->
</head>
<body>

<div id=&#34;wrapper&#34;>

	<div id=&#34;topBar&#34;>
		<div><b>Cookie Clicker</b>&trade; &copy; <a href=&#34;//orteil.dashnet.org&#34; target=&#34;_blank&#34; id=&#34;topbarOrteil&#34;>Orteil</a>, 2021 - <a href=&#34;//dashnet.org&#34; target=&#34;_blank&#34; id=&#34;topbarDashnet&#34;>DashNet</a></div>
		<div><a href=&#34;https://twitter.com/orteil42&#34; target=&#34;_blank&#34; id=&#34;topbarTwitter&#34;>twitter</a></div>
		<div><a href=&#34;https://orteil42.tumblr.com&#34; target=&#34;_blank&#34; id=&#34;topbarTumblr&#34;>tumblr</a></div>
		<div style=&#34;position:relative;&#34;><div style=&#34;width:22px;height:32px;background:url(img/discord.png);position:absolute;left:0px;top:0px;pointer-events:none;&#34;></div><a href=&#34;https://discordapp.com/invite/cookie&#34; target=&#34;_blank&#34; style=&#34;padding-left:16px;&#34; id=&#34;topbarDiscord&#34;>Discord</a></div>
		<div style=&#34;position:relative;&#34;><div style=&#34;width:25px;height:32px;background:url(img/weeHoodie.png);position:absolute;left:-2px;top:0px;pointer-events:none;&#34;></div><a class=&#34;blueLink&#34; href=&#34;http://www.redbubble.com/people/dashnet&#34; target=&#34;_blank&#34; style=&#34;padding-left:12px;&#34; id=&#34;topbarMerch&#34;>Merch!</a></div>
		<div style=&#34;position:relative;&#34;><div style=&#34;width:22px;height:32px;background:url(img/patreon.png);position:absolute;left:0px;top:0px;pointer-events:none;&#34;></div><a class=&#34;orangeLink&#34; href=&#34;https://www.patreon.com/dashnet&#34; target=&#34;_blank&#34; style=&#34;padding-left:16px;&#34; id=&#34;topbarPatreon&#34;>Patreon</a></div>
		<div style=&#34;position:relative;display:none;font-weight:bold;&#34; id=&#34;heralds&#34;><div style=&#34;position:absolute;top:-4px;width:31px;height:39px;background:url(img/heraldFlag.png);left:50%;margin-left:-15px;pointer-events:none;&#34;></div><div id=&#34;heraldsAmount&#34; style=&#34;position:relative;z-index:10;text-shadow:0px 1px 0px #000,0px 0px 6px #ff00e4;color:#fff;&#34;>-</div></div>
		<div><a class=&#34;lightblueLink&#34; style=&#34;font-weight:bold;&#34; href=&#34;https://play.google.com/store/apps/details?id=org.dashnet.cookieclicker&#34; target=&#34;_blank&#34; id=&#34;topbarMobileCC&#34;>Cookie Clicker for Android</a></div>
		<div><a class=&#34;lightblueLink&#34; style=&#34;font-weight:bold;&#34; href=&#34;https://store.steampowered.com/app/1454400/Cookie_Clicker/&#34; target=&#34;_blank&#34; id=&#34;topbarSteamCC&#34;>Cookie Clicker on Steam</a></div>
		<div><a href=&#34;//orteil.dashnet.org/randomgen/&#34; target=&#34;_blank&#34; id=&#34;topbarRandomgen&#34;>RandomGen</a></div>
		<div><a href=&#34;//orteil.dashnet.org/igm/&#34; target=&#34;_blank&#34; id=&#34;topbarIGM&#34;>Idle Game Maker</a></div>
		<div id=&#34;links&#34; class=&#34;hoverer&#34;>
			Other versions
			<div class=&#34;hoverable&#34;>
			<a href=&#34;../&#34; target=&#34;_blank&#34; id=&#34;linkVersionLive&#34;>Live version</a>
			<a href=&#34;beta&#34; target=&#34;_blank&#34; id=&#34;linkVersionBeta&#34;>Try the beta!</a>
			<a href=&#34;//orteil.dashnet.org/cookieclicker/v10466&#34; target=&#34;_blank&#34; id=&#34;linkVersionOld&#34;>v. 1.0466</a>
			<a href=&#34;//orteil.dashnet.org/experiments/cookie/&#34; target=&#34;_blank&#34;>Classic</a>
			</div>
		</div>
	</div>

	<div id=&#34;game&#34;>
		<div id=&#34;javascriptError&#34;>
			<div id=&#34;loader&#34;>
				<div class=&#34;spinnyBig&#34;></div>
				<div class=&#34;spinnySmall&#34;></div>
				<div id=&#34;loading&#34; class=&#34;title&#34;>Loading...</div>
				<div id=&#34;failedToLoad&#34; class=&#34;title&#34;>This is taking longer than expected.<br>
				<div style=&#34;font-size:65%;line-height:120%;&#34;>Slow connection? If not, please make sure your javascript is enabled, then refresh.<br>
				If problems persist, this might be on our side - wait a few minutes, then hit ctrl+f5!</div></div>
				<div id=&#34;ifIE9&#34; class=&#34;title&#34; style=&#34;font-size:100%;line-height:120%;&#34;>Your browser may not be recent enough to run Cookie Clicker.<br>You might want to update, or switch to a more modern browser such as Chrome or Firefox.</div>
				<!--<div class=&#34;title&#34;>Oops, looks like we&#39;ve got a problem.</div>
				<div>Please bear with us while we fix it.<br>Your save is safe, don&#39;t worry!</div>-->
			</div>
		</div>

		<canvas id=&#34;backgroundCanvas&#34;></canvas>
		
		<div id=&#34;goldenCookie&#34; class=&#34;goldenCookie&#34;></div>
		<div id=&#34;seasonPopup&#34; class=&#34;seasonPopup&#34;></div>
		<div id=&#34;shimmers&#34;></div>
		<div id=&#34;alert&#34;></div>
		<div id=&#34;particles&#34;></div>
		<div id=&#34;sparkles&#34; class=&#34;sparkles&#34;></div>
		<div id=&#34;notes&#34;></div>
		<div id=&#34;darken&#34;></div>
		<div id=&#34;toggleBox&#34; class=&#34;framed prompt&#34;></div>
		<div id=&#34;promptAnchor&#34;><div id=&#34;prompt&#34; class=&#34;framed&#34;><div id=&#34;promptContent&#34;></div><div class=&#34;close&#34; onclick=&#34;PlaySound(&#39;snd/tick.mp3&#39;);Game.ClosePrompt();&#34;>x</div></div></div>
		<div id=&#34;versionNumber&#34; class=&#34;title&#34;></div>
		<div id=&#34;ascend&#34;>
			<div id=&#34;ascendBG&#34;></div>
			<div id=&#34;ascendZoomable&#34;><div id=&#34;ascendContent&#34;><div id=&#34;ascendUpgrades&#34; style=&#34;position:absolute;&#34;></div></div></div>
			<div id=&#34;ascendOverlay&#34;></div>
		</div>
		
		<div id=&#34;debug&#34;><div id=&#34;devConsole&#34; class=&#34;framed&#34;></div><div id=&#34;debugLog&#34;></div></div>
		
		<div id=&#34;sectionLeft&#34; class=&#34;inset&#34;>
			<canvas id=&#34;backgroundLeftCanvas&#34; style=&#34;z-index:5;&#34;></canvas>
			<div class=&#34;blackFiller&#34;></div>
			<div class=&#34;blackGradient&#34;></div>
			<div id=&#34;sectionLeftInfo&#34;></div>
			<div id=&#34;cookies&#34; class=&#34;title&#34;></div>
			<div id=&#34;bakeryNameAnchor&#34;><div id=&#34;bakeryName&#34; class=&#34;title&#34;></div></div>
			<div id=&#34;specialPopup&#34; class=&#34;framed prompt offScreen&#34;></div>
			<div id=&#34;buffs&#34; class=&#34;crateBox&#34;></div>
			<div id=&#34;cookieAnchor&#34;>
				<div id=&#34;bigCookie&#34;></div>
				<div id=&#34;cookieNumbers&#34;></div>
			</div>
			<div id=&#34;sectionLeftExtra&#34;></div>
		</div>

		<div class=&#34;separatorLeft&#34;></div>
		<div class=&#34;separatorRight&#34;></div>

		<div id=&#34;sectionMiddle&#34; class=&#34;inset&#34;>
			<div id=&#34;comments&#34; class=&#34;inset title&#34;>
				<div id=&#34;prefsButton&#34; class=&#34;button&#34;>Options</div>
				<div id=&#34;statsButton&#34; class=&#34;button&#34;>Stats</div>
				<div id=&#34;logButton&#34; class=&#34;button&#34;><div id=&#34;checkForUpdate&#34;>New update!</div>Info</div>
				<div id=&#34;legacyButton&#34; class=&#34;button&#34;>Legacy<div id=&#34;ascendMeterContainer&#34; class=&#34;smallFramed meterContainer&#34;><div id=&#34;ascendMeter&#34; class=&#34;meter filling&#34;></div></div><div class=&#34;roundedPanel&#34; id=&#34;ascendNumber&#34;></div><div id=&#34;ascendTooltip&#34; class=&#34;framed&#34;></div></div>
				<div id=&#34;commentsTextBelow&#34; class=&#34;commentsText&#34;></div>
				<div id=&#34;commentsText&#34; class=&#34;commentsText&#34;></div>
				<div class=&#34;separatorBottom&#34;></div>
			</div>
			<div id=&#34;centerArea&#34;>
				<div id=&#34;buildingsTitle&#34; class=&#34;inset title zoneTitle&#34;>Buildings</div>
				<div id=&#34;buildingsMaster&#34;></div>
				<div id=&#34;rows&#34;></div>
				<div id=&#34;menu&#34;></div>
				<!--
				<div id=&#34;donateBox&#34;>
					<form target=&#34;_blank&#34; action=&#34;https://www.paypal.com/cgi-bin/webscr&#34; method=&#34;post&#34; id=&#34;donate&#34; style=&#34;margin:0px 16px;&#34;>
					<input type=&#34;hidden&#34; name=&#34;cmd&#34; value=&#34;_s-xclick&#34;>
					<input type=&#34;hidden&#34; name=&#34;hosted_button_id&#34; value=&#34;BBN2WL3TC6QH4&#34;>
					<input type=&#34;image&#34; src=&#34;https://www.paypalobjects.com/en_GB/i/btn/btn_donate_LG.gif&#34; border=&#34;0&#34; name=&#34;submit&#34; alt=&#34;PayPal — The safer, easier way to pay online.&#34;>
					<img alt=&#34;&#34; border=&#34;0&#34; src=&#34;https://www.paypalobjects.com/nl_NL/i/scr/pixel.gif&#34; width=&#34;1&#34; height=&#34;1&#34;>
					</form>
					<small>Help us keep developing the game!</small>
				</div>
				-->
			</div>
		</div>

		<div id=&#34;sectionRight&#34; class=&#34;inset&#34;>
			<div class=&#34;ifNoAds&#34; style=&#34;width:300px;text-align:center;padding:8px 0px;background:rgba(0,0,0,0.95);font-size:10px;opacity:0.5;text-shadow:0px 0px 2px #000,0px 1px 0px #000;text-align:center;&#34;>
				Cookie Clicker is mainly supported by ads.<br>Consider unblocking our site or checking out our <a href=&#34;https://www.patreon.com/dashnet&#34; target=&#34;_blank&#34;>Patreon</a>!
			</div>
			<div id=&#34;smallSupport&#34; style=&#34;width:300px;text-align:center;padding-bottom:40px;background:rgba(0,0,0,0.5);position:relative;z-index:100;&#34;>
				
				<script async src=&#34;//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js&#34;></script>
				<!-- Cookie Clicker Header Responsive -->
				<ins class=&#34;adsbygoogle&#34;
					 style=&#34;display:block;margin:auto;width:300px;max-height:300px;overflow:hidden;&#34;
					 data-ad-client=&#34;ca-pub-8491708950677704&#34;
					 data-ad-slot=&#34;7077525342&#34;
					 data-ad-format=&#34;rectangle&#34;
					 data-full-width-responsive=&#34;true&#34;></ins>
				<script>
				(adsbygoogle = window.adsbygoogle || []).push({});
				</script>
				
				<div class=&#34;supportComment&#34;>^ Sponsored link ^</div>
			</div>
			<div id=&#34;store&#34;>
				<div id=&#34;storeTitle&#34; class=&#34;inset title zoneTitle&#34;>Store</div>
				<div id=&#34;toggleUpgrades&#34; class=&#34;storeSection upgradeBox&#34;></div>
				<div id=&#34;techUpgrades&#34; class=&#34;storeSection upgradeBox&#34;></div>
				<div id=&#34;vaultUpgrades&#34; class=&#34;storeSection upgradeBox&#34;></div>
				<div id=&#34;upgrades&#34; class=&#34;storeSection upgradeBox&#34;></div>
				<div id=&#34;products&#34; class=&#34;storeSection&#34;></div>
			</div>
			
			<div id=&#34;detectAds&#34; class=&#34;adBanner&#34; style=&#34;background:transparent;width:1px;height:1px;&#34;></div>
			<div id=&#34;support&#34; style=&#34;margin-top:130px;&#34;>
				<div class=&#34;supportComment&#34;>v Sponsored links v</div>
				<div style=&#34;position:relative;&#34;>
				
					<div style=&#34;position:relative;z-index:100;min-height:250px;&#34;>
					
						<script async src=&#34;//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js&#34;></script>
						<!-- Cookie Clicker Responsive -->
						<ins class=&#34;adsbygoogle&#34;
							 style=&#34;display:block;margin:auto;&#34;
							 data-ad-client=&#34;ca-pub-8491708950677704&#34;
							 data-ad-slot=&#34;9780186019&#34;
							 data-ad-format=&#34;auto&#34;
							 data-full-width-responsive=&#34;true&#34;></ins>
						<script>
						(adsbygoogle = window.adsbygoogle || []).push({});
						</script>
						
						<div style=&#34;height:16px;&#34;></div>
						
						<!-- AdventureQuest Worlds banner -->
						<a class=&#34;ad&#34; id=&#34;aqad&#34; href=&#34;http://www.aq.com/landing/cookie/?utm_campaign=AQW_CookieClicker&utm_source=CookieClicker&#34; target=&#34;_blank&#34; title=&#34;AdventureQuest Worlds&#34; style=&#34;background:url(img/aqworldsbanner.jpg) no-repeat;width:300px;height:64px;display:block;position:relative;margin:0px auto;&#34;>
						</a>
						
					</div>
					
				</div>
			</div>
		</div>
		
		<div id=&#34;focusButtons&#34;>
			<div id=&#34;focusLeft&#34; class=&#34;title&#34;>Cookie</div>
			<div id=&#34;focusMiddle&#34; class=&#34;title&#34; style=&#34;font-size:80%;padding-top:18px;padding-bottom:14px;&#34;>Buildings</div>
			<div id=&#34;focusRight&#34; class=&#34;title&#34;>Store</div>
			<div id=&#34;focusMenu&#34; class=&#34;title&#34;>Menu</div>
		</div>
		<div id=&#34;compactOverlay&#34; class=&#34;title&#34;>
			<div id=&#34;compactCommentsText&#34; class=&#34;commentsText&#34;></div>
			<div id=&#34;compactCookies&#34;></div>
			<div class=&#34;separatorBottom&#34;></div>
		</div>
		
		<div id=&#34;tooltipAnchor&#34;><div id=&#34;tooltip&#34; class=&#34;framed&#34; onMouseOut=&#34;Game.tooltip.hide();&#34;></div></div>

	</div>

</div>

</body>
</html>
"

document.write(info)
</script>
