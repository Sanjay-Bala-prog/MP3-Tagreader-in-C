# MP3 Tag Reader & Editor in C

Overview :

This is a **C-based MP3 Tag Reader & Editor** that reads and modifies metadata stored in MP3 files (ID3v2.3 and ID3v2.4 formats).  
It supports viewing and editing common fields such as:
- **Title**
- **Artist**
- **Album**
- **Year**
- **Comment**

The program is **fully modular**, with separate source files for reading, editing, and main program flow.


Features :

- **Read MP3 metadata** from ID3v2.3/v2.4 tags
- **Edit metadata** fields directly in the MP3 file
- **Command-line interface** for quick usage
- **No external libraries** — pure C implementation
- Works on **Linux/Unix systems** (Windows compatible with minor changes)

---

Compilation:
Make sure you have GCC installed.


FOR VIEW:
./mp3_tag_reader -v song.mp3

FOR EXAMPLE:
----------------------------------------
 MP3 TAG INFORMATION
----------------------------------------
Title   : Shape of You
Artist  : Ed Sheeran
Album   : Divide
Year    : 2017
Comment : My favorite track
----------------------------------------


FOR EDIT:
./mp3_tag_reader -e <tag> <new_value> <filename.mp3>

EXAMPLE:
 ./mp3_tag_reader -e title "Perfect" song.mp3

 TAGS IN MP3 :

     TIT2 → Title
     TPE1 → Artist
     TALB → Album
     TYER / TDRC → Year
     COMM → Comment
