const cacheName = "thingmaker-minekhan-cache3"
const assets = [
  "/",
  "/index.html",
  "/favicon.ico",
  "/maps.json",
  "/mojangles.ttf"
]

function canCache(url) {
	return url.startsWith("https://data.thingmaker.repl.co/") && !url.startsWith("https://data.thingmaker.repl.co/sounds/music/")
}
function cacheForever(url){
  return url.startsWith("https://data.thingmaker.repl.co/sounds/")
}

self.addEventListener("install", event => {
  self.skipWaiting()

  event.waitUntil(
    caches.open(cacheName).then(cache => {
      cache.addAll(assets)
    })
  )
})
self.addEventListener("activate", event => {
  event.waitUntil(caches.keys().then(keys => {
    return Promise.all(keys.map(key => {
      if (key !== cacheName) {
        return caches.delete(key)
      }
    }))
  }))
})
self.addEventListener("fetch", event => {
  event.respondWith(
    caches.open(cacheName).then(cache => {
      let url = event.request.url
      return caches.match(event.request).then(res => {
        if(res && cacheForever(url)) return res
        return fetch(event.request).then(fetchRes => {
          if (canCache(url)) {
            cache.put(event.request, fetchRes.clone())
          }
          return fetchRes
        }).catch(() => res)
      })
    })
  )
})