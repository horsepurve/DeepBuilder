# DeepBuilder: Deep Learning for English Vocabulary Building
## Introduction
DeepBuilder is an artificial intelligence-based vocabulary builder with Qt5 GUI which can help you remember English words. Current approaches for vocabulary building are neither logical nor efficient. You remember words one by one discretely without knowing their connections, context, and importance.
</br>DeepBuilder, instead, can systematically improve your vocabulary remembering, by means of the state-of-the-art deep learning techniques. DeepBuilder can help you with its following unique functions, note that some of which are still under development:
* DeepBuilder was firstly trained on large corpuses and then fine-tuned on small corpuses using transfer learning due to the limited data for a specific exam. So you can chose your target exam when using DeepBuilder.
* Each word has its forgetting curve and each person has his own forgetting curve for the same word. DeepBuilder collects forgetting curves for every word from every people. It will recommend words to you based on both corpus and your personalized model. 
* Every word is placed in the word network and DeepBuilder will recommend the word easiest for you to remember based on your currently learned words using network algorithm.
* Theoretically, building a behemoth word bank needs you to ingest tons of material. However, DeepBuilder can accelerate this process for you using the advanced AI algorithms.
## How to Use
### Installation
No installation is needed. Decompress the zip file in bin directory and click the exe file. You can specify the vocabulary file in config.json and the vocabulary file will be backed up automatically. 
### Tips
There are several tips using DeepBuilder, and following is the maim panel of the software:
</br><div align="center"><img src="https://github.com/horsepurve/DeepBuilder/blob/main/img/WordLink1.png" width="60%" alt="figure_1" /></div>
* Add a new word to the list: type in the box and click <b>Add Word</b>
* Add a new note to current word: type in the box and click <b>Add Note</b>
* Search whether a word is in the list: type in the box and click <b>Search</b>
* Link the current word to another word: type another word in the box and click <b>Link Word</b>
* See your added words for each day: click <b>Analysis</b>:
</br><div align="center"><img src="https://github.com/horsepurve/DeepBuilder/blob/main/img/WordLink2.png" width="60%" alt="figure_1" /></div>

### Build from Source
You need Qt5 to compile the source code. 

Development date: Apr. 2016.
