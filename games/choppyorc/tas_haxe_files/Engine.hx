package;

import haxe.Json;
import KeyBindings;
import js.Browser;
import haxe.ds.Option;

class PlayControl {
	public var frame:Int = 0; // Frames since level start. Inputs do happen on frame 0!
	public var paused:Bool = false; // If true, wait for frame advance to run.
	public var speed:Int = 0; // 0 for slow, 1 for normal, 2 for fast forward.
	public var silent:Bool = false; // Hide input messages?

	public function new() {}

	public function pause() {
		paused = true;
		speed = 0;
	}
}

class Engine {
	var frameLength:Float = 1000.0 / 60.0;

	var control = new PlayControl();
	var playback:Option<Video.VideoPlayer> = None; // If this is initialized, we're in playback.
	var recording:Video.VideoRecorder = new Video.VideoRecorder(0);
	var slots:Array<Video>;

	var fullgameVideo:Dynamic = null; // Special level-by-level videos for eddynardo.
	var fullgameLevelCounter:Int = 0;

	var pausedCallback:Option<Dynamic> = None;
	var fakeTime:Float = 0;

	var _requestAnimationFrame:Dynamic;
	var _now:Dynamic;
	var initialDirection = 0;

	public function new() {
		// Inject our methods into the global scope.
		_requestAnimationFrame = Browser.window.requestAnimationFrame;
		_now = Browser.window.performance.now;
		untyped window.requestAnimationFrame = this.requestAnimationFrame;
		untyped window.performance.now = function() {
			return fakeTime;
		}

		// hook into the helper script
		untyped window.coffee = {};
		untyped window.coffee._onScene = onScene;

		untyped window.coffee._keyup = this.keyup;
		untyped window.coffee._keydown = this.keydown;

		// API for runners
		untyped window.coffee.load = function(string:String) {
			slots[0] = new Video(string);
		}
		untyped window.coffee.loadFullGame = function(strings:Array<String>) {
			fullgameVideo = strings.map(function(videoString) {
				return new Video(videoString);
			});
		}
		untyped window.coffee.clearFullGame = function(string:String) {
			fullgameVideo = null;
		}
		untyped window.coffee.startLeft = function() {
			initialDirection = 1;
		}
		untyped window.coffee.startRight = function() {
			initialDirection = 2;
		}
		untyped window.coffee.startNeutral = function() {
			initialDirection = 0;
		}
		untyped window.coffee.useFrame = function(fl) {
			frameLength = fl;
		}

		fakeTime = _now();

		slots = new Array();
		for (i in 0...10) {
			slots.push(new Video());
		}

		control.speed = 1;
	}

	function wrapCallback(callback:Dynamic) {
		return function() {
			fakeTime += frameLength;

			switch playback {
				case Some(player):
					for (action in player.getActions(control.frame)) {
						sendGameInput(action.code, action.down);
					}
					if (control.frame + 1 >= player.video.pauseFrame) {
						// playback is over

						if (fullgameVideo == null) {
							// normally, pause at the last frame of a video
							control.pause();
							trace('[PAUSE ] @ ${control.frame + 1}');
							control.silent = false;
						} else {
							// for fullgame playback, prime the initial direction controls

							if (fullgameLevelCounter >= 1 && fullgameLevelCounter <= 15) {
								initialDirection = fullgameVideo[fullgameLevelCounter - 1].initialDirection;
								control.frame = 0;
								primeControls(true);
							}
						}

						playback = None;
					}
					callback(fakeTime);
				case None:
					callback(fakeTime);
			}

			control.frame += 1;
		}
	}

	function requestAnimationFrame(callback:Dynamic) {
		var wrappedCallback = wrapCallback(callback);
		if (!control.paused) {
			switch control.speed {
				case 0:
					Browser.window.setTimeout(wrappedCallback, 100);
				case 1:
					_requestAnimationFrame(wrappedCallback);
				case _:
					Browser.window.setTimeout(wrappedCallback, 0);
			}
		} else {
			pausedCallback = Some(wrappedCallback);
		}
	}

	function triggerPausedCallback() {
		switch pausedCallback {
			case Some(cb):
				pausedCallback = None;
				cb();
			case None:
				{}
		}
	}

	var keyupHandler:Dynamic;
	var keydownHandler:Dynamic;

	function keyup(callback:Dynamic) {
		keyupHandler = callback;
		Browser.window.onkeyup = function(key) {
			onKey(key, false);
		}
	}

	function keydown(callback:Dynamic) {
		keydownHandler = callback;
		Browser.window.onkeydown = function(key) {
			onKey(key, true);
		}
	}

	// Top-level for keyboard input from the user.
	function onKey(event:Dynamic, down:Bool) {
		if (!Util.isSome(playback)) {
			// We're not in playback, so we should pass through keys.
			var suppress = [83, 87, 65, 68, 82]; // prevent pressing alternate movement keys and 'r'
			if (suppress.indexOf(event.keyCode) == -1)
				sendGameInput(event.keyCode, down);
		}
		if (down && fullgameVideo == null) {
			switch (KeyBindings.fromKeyCode(event.keyCode)) {
				case Some(input):
					if (handleInterfaceInput(input, event.ctrlKey, event.altKey)) {
						event.preventDefault();
					}
				case _:
					{}
			}
		}
	}

	// Send input to the game and record it.
	function sendGameInput(keyCode:Int, down:Bool) {
		recording.recordKey(control.frame, keyCode, down, control.silent);
		var event = {which: keyCode, preventDefault: function() {}};
		if (down) {
			keydownHandler(event);
		} else {
			keyupHandler(event);
		}
	}

	function primeControls(buffer:Bool) {
		for (code in Video.keyCodes) {
			sendGameInput(code, false);
		}
		if (initialDirection == 1) {
			if (buffer)
				trace("---> Holding controls: LEFT.");
			sendGameInput(37, true);
		}
		if (initialDirection == 2) {
			if (buffer)
				trace("---> Holding controls: RIGHT.");
			sendGameInput(39, true);
		}
		if (initialDirection == 0) {
			if (buffer)
				trace("---> Holding controls: NONE.");
		}
	}

	function resetLevel(?slot:Int, ?replay:Bool) {
		if (replay == null)
			replay = false;
		trace('[${replay ? "REPLAY" : "RESET to"} ${(slot == null) ? "start" : "slot " + Std.string(slot) + "..."}]');
		sendGameInput(82, true);
		sendGameInput(82, false);
		recording = new Video.VideoRecorder(initialDirection);
		control = new PlayControl();
		primeControls(true);
	}

	function loadPlayback(video:Video) {
		playback = Some(new Video.VideoPlayer(video));
		initialDirection = video.initialDirection;
		recording = new Video.VideoRecorder(initialDirection);
	}

	// Keyboard interface.
	// Return true to signal that input was captured.
	function handleInterfaceInput(input:KeyBindings.CoffeeInput, ctrlKey:Bool, altKey:Bool):Bool {
		var oldControl = untyped JSON.parse(JSON.stringify(control));

		// stepping frames
		if (input == CoffeeInput.StepFrame && control.paused) {
			trace('[STEP  ] @ ${control.frame + 1}');
			triggerPausedCallback();
			return true;
		}

		// pausing
		if (input == CoffeeInput.Pause) {
			if (!oldControl.paused)
				trace('[PAUSE ] @ ${control.frame + 1}');
			control.pause();
			return true;
		}

		// changing playback speed
		{
			var playAction = true;
			switch (input) {
				case CoffeeInput.PlaySlow:
					control.speed = 0;
				case CoffeeInput.PlayNormal:
					control.speed = 1;
				case CoffeeInput.PlayFast:
					control.speed = 2;
				case _:
					playAction = false;
			}
			if (playAction) {
				control.paused = false;
				if (oldControl.paused)
					trace('[PLAY  ] @ ${control.frame}');
				triggerPausedCallback();
				return true;
			}
		}

		// r to reset level
		if (input == CoffeeInput.Reset) {
			playback = None;
			resetLevel();
			control.pause();
			triggerPausedCallback();
			return true;
		}

		// p to replay the video in slot 0 at normal speed
		if (input == CoffeeInput.Replay) {
			loadPlayback(slots[0]);
			resetLevel(0, true);
			control.speed = 1;
			triggerPausedCallback();
			return true;
		}

		// handling slots
		switch (input) {
			case CoffeeInput.Slot(slot):
				// replay slot
				if (!ctrlKey) {
					loadPlayback(slots[slot]);
					resetLevel(slot);
					control.speed = 2;
					if (altKey)
						control.pause();
					control.silent = true;
					triggerPausedCallback();
					return true;
				}

				// with ctrl: save slot
				if (ctrlKey && !altKey) {
					control.pause();
					var video = recording.saveVideo(control.frame);
					trace('[SAVE slot ${slot}] @ ${control.frame}');
					trace('data: ${video.toString()}');
					slots[slot] = video;
					return true;
				}
			case _:
				{}
		}
		return false;
	}

	function onScene(levelNum:Int) {
		trace('[SCENE ${levelNum}]');
		if (fullgameVideo != null && fullgameVideo.length >= levelNum) {
			fullgameLevelCounter = levelNum;
			loadPlayback(fullgameVideo[fullgameLevelCounter - 1]);
			control.paused = false;
			control.frame = 0;
			control.speed = 1;
			primeControls(false);
		}
	}
}
