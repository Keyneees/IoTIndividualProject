# IoT Individual Project - Signal Sampling
<table>
    <tr>
        <th><b>Name:</b></th>
        <th><b>Surname:</b></th>
        <th><b>Id:</b></th>
        <th><b>Email:</b></th>
    </tr>
    <tr>
        <td>Michele</td>
        <td>Nicoletti</td>
        <td>1886646</td>
        <td>nicoletti.1886646@studenti.uniroma1.it</td>
    </tr>
</table>

<h1>PROJECT GOAL</h1>
The project takes as input a sinusoid, analyzes that wave looking for its frequency and optimal frequency and communicates the average of the input wave sampled to a mqtt broker.

With optimal frequency

<h2>PROJECT ORGANIZATION</h2>
This project has multiple <i>file.h</i>. In each of them is developed a specific main functionality:
<ul>
    <li><code>input_signal.h</code>, in which there are all the function for the management of the input signal;</li> 
    <li><code>fft.h</code>, in which there is the function that executes the <i>Fast Fourier Transform</i> of the first input signal detected by the code;</li> 
    <li><code>mqtt.h</code>, that contains the instruction for <i>WiFi connection</i> and <i>MQTT communication</i>;</li>
    <li><code>certificate.h</code> has a vector with the encrypted values of the certificate required for a <b>secure</b> MQTT communication;</li>
    <li><code>common.h</code> contains all the common <code>#include</code> shared between the files.</li>
</ul>
The last file of the project is <code>main.c</code>, that manages the  project execution:
<ol>
    <li> initialization of input element and first data sample;</li>
    <li> computation of FFT of input sampled and max frequency of the signal;</li>
    <li> computation of the average of input signal;</li>
    <li> communication of average to a mqtt broker;</li>
    <li> beginning of loop cycle that samples the signal over a specific window, based on max frequency computed, and communication of the average determined thanks to an <i>aggregate function</i>.</li>
</ol>

