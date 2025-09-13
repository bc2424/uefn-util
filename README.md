# uefn-util
UEFN-util is a DLL that intends to provide an easy to set up and use Unreal Engine experience for Fortnite modding.
Supporting 31.41 UEFN, it allows you to load ingame in the Battle Royale gamemode using custom items, weapons, and skins!

# CURRENT 31.41 + UEFN DOWNLOAD
https://gofile.io/d/5KnfUv

## HOW TO USE:
1. Compile the "UbaHost.dll" or get the latest version from [releases](https://github.com/bc2424/uefn-util/releases/).
2. Place the DLL "UbaHost.dll" in the path "Engine\Binaries\Win64\UnrealBuildAccelerator\x64\" in your install.
   *  "UnrealBuildAccelerator\x64\" will not exist by default.
3. [Download Neonite from Hybrid FNBR's github](https://github.com/HybridFNBR/Neonite). Follow the readme then run the `Start Neonite.bat` file.
4. Launch UEFN from the shipping exe in FortniteGame\Binaries\Win64\ with Neonite open, if the DLL was placed correctly, you will load into the unlocked editor.
5. To load ingame, click Edit in the top left, go to Editor Preferences... scroll to Play Credentials and Enable Logins then click add Credentials, user id should be a username with a url at the end, password will be the same username (password is hidden), type should be set to exchangecode.
  * example:
    * User ID: businesscow245@og.com
    * Password: businesscow245
    * Type: exchangecode
6. Load into a map with the gamemode set to Athena_GameMode (/Game/Athena/Maps/Athena_EmptyTest is a good example) click the green play button, you should succesfully load in.

## Features:
* In-game on the Battle Royale gamemode.
* Building, Editing, and Vehicles.
* All weapons and items (some only working to an extent).
* Equip any cosmetic from the Cosmetic Cheats.
* Ability to open and preview any cooked assets.

## FAQ
 * What is UEFN-PIE?
      * UEFN-PIE means "Play in Editor for UEFN". Play in Editor is a mode in Unreal Engine that allows you to test your game from within the Unreal Editor. The way UEFN is made allows you to use PIE provided the editor is patched to load it without issues. UEFN-Util does everything required to get the mode working automatically.
  * Will I get banned from Fortnite if I use this?
      * You will not get banned. UEFN-Util can potentially interfere with files in your UEFN local appdata, but it does not connect to Epic servers, and will not touch anything in your existing Fortnite install. Epic has never taken action against people using PIE, even on the latest Fortnite versions.
  * Can I use this on the latest version of Unreal Editor for Fortnite?
      * No. UEFN-Util only works with 31.41 (Chapter 5 Season 4).
  * Can I load the Chapter 5/Chapter 3 map?
      * You are able to preview the Chapter 5 map but you are ***currently not*** able to load into it by attempting to start PIE (which will crash), this will be fixed in the future. You are able to load into and play on the Chapter 3 map.
  * How do I get Weapons/Items?
      * Items can be granted in several ways. Starting items can be added by going into Editor Preferences -> Battle Royale Settings -> Inventory Items to Grant, and you can use the console commands `cheat giveweapon` or `cheat givespecificitem` to give certain items midgame, example usage of the commands is provided below.
  * Can I load into the Lobby?
      * Yes. Start PIE from the map `/Game/Maps/Frontend` and you will load into the lobby, Net Mode must be set to standalone and the game will inform you of this.
  * How do I make pressing Escape not close PIE?
      * Search for Stop Simulation in Editor Preferences and unbind Escape.
  * Can I play other gamemdes?
      * Save the World and Rocket Racing are currently unimplemented, and Neonite does not support Save the World. Lego Fortnite and Fortnite Festival do not contain enough code to work properly on 31.41 UEFN.
   
## Troubleshooting
 * I get the error "Failed to login" or "A new Unreal Editor for Fortnite version is avaliable." when I launch UEFN.
      * You forgot to put the UbaHost.dll in the "Engine\Binaries\Win64\UnrealBuildAccelerator\x64\" folder listed in the how to use section.
 * When I press play nothing happens and the play button is greyed out.
      * Known to happen the first time you launch UEFN if you are missing play credentials. Restart the editor with Neonite running and try again.
 * My keybinds don't save when I relaunch PIE.
      * Neonite issue that UEFN cannot do anything about. Use a different backend instead if you would prefer keybinds and you know how to do this.
 * My game crashes when I search for assets in the content browser.
      * Likely due to the game loading an animsequence, which can crash if it is cooked. This will be fixed later. Search with the filter `-animsequence` so they will be ignored.
 * I can't open a certain asset or my game crashes when I try to open/modify assets.
      * Cooked content has several issues like this, ignore trying to modify or save certain cooked assets like meshes, materials, maps directly. If you get a "failed to open asset" error when double clicking on assets, try using the enter key to confirm the selection.
 * My game keeps crashing when I do this or that!
      * Know UEFN is not designed to support PIE, it can be unstable if you do certain actions, tell us what you are doing to cause a crash and we might be able to help.
 * When loading ingame, I can't move and I have no skin, or I don't drop loading screen, and the error `OnProtocolMismatchReported` is logged in the console.
      * There is a likely a game feature plugin mismatch, run the command `ListGameFeaturePlugins -activeonly` and compare with the editor to see if there are different plugins active, you can go to the plugin's GameFeatureData and set it to Registered or Loaded to disable the plugins, some plugins rely on other plugins, if you get it consistently, wipe your UEFN appdata because there is probably an issue in there.
  
## Planned features
 * World Partition fixes + loading cooked maps with `servertravel`
 * Automatic loading of certain assets missing from editor registry. (like the SaveTheWorld folder)
 * Better codebase for STW support.
 * Make anims sequences not crash when their thumbnail is loaded in editor.
 * Fixes for several things that work badly or don't work at all.
 * Loading of compiled Mutables
 * Re-enable cosmetic streaming in editor
 * Aircraft and storm
 * Player bots

## Commands
Commands can be entered once in game by opening the console and entering them as cheats.
* Example: `cheat giveweapon WID_Shotgun_Swing_Athena_SR`

| Command | Description | Example | 
| ----------- | ----------- | ----------- |
| giveweapon Weapon | Gives any PrimaryAssetType "Weapon" item through its PrimaryAssetName. | cheat giveweapon WID_Shotgun_Swing_Athena_SR |
| giveathenagadget AthenaGadget | Gives any PrimaryAssetType "AthenaGadget" item through its PrimaryAssetName. | cheat giveathenagadget AGID_AshtonPack_Chicago |
| givegadget Gadget | Gives any PrimaryAssetType "Gadget" item through its PrimaryAssetName. | cheat givegadget WID_HighTower_Plum_KineticAbs_CoreBR |
| giveconsumable Consumable | Gives any PrimaryAssetType "Consumable" item through its PrimaryAssetName. | cheat giveconsumable Athena_ChillBronco |
| givetrap Trap | Gives any PrimaryAssetType "Trap" item through its PrimaryAssetName. | cheat givetrap TID_Context_Freeze_Athena |
| givespecificitem ObjectPath | Gives any WorldItem from its object path. Better to use if other commands give you issues. | cheat givespecificitem /Game/Athena/Items/Weapons/WID_Shotgun_Swing_Athena_SR.WID_Shotgun_Swing_Athena_SR |
| toggleinfiniteammo | Toggle Infinite Ammo and Free Building on and off. | cheat toggleinfiniteammo |
| sethealthpercent Value | Set Health and Max Health to any value. | cheat sethealthpercent 100 |
| setshieldpercent Value | Set Shield and Max Shield to any value. | cheat setshieldpercent 100 |

## Credits
* simonhxd - Pickup code, fixes, secondary project owner. 
* Twin1 - Codebase built off [their 11.31 gameserver](https://github.com/Twin1dev/11.31-Gameserver).
* Milxnor - Cobalt used directly for redirecting to Neonite. Some ingame code taken from Project Reboot.
* Heliato - Some code derived from [their 19.10 gameserver](https://github.com/Heliato/HalalGS-19.10).
* Trifecta, gamerbross, jaiz, pengu1ns - Miscellaneous help.
* AllyJax and lolLucoa - Playtesting.
* HybridFNBR - Current Neonite maintainer.
