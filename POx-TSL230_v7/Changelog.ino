/*
 * 5.9.: Changed the loop with if()s and disregarding the timerOne-ISR
 * 
 * 
 * 6.9.: Moved the time-compare-if-loop from the isr (add_pulse) outside for better signal and faster performance.
 * best divide-by factor is seen as 2; highest sensitivity level 100 needed; still erroronueous HR's 222,166,333, ....
 * old functions:
 *  add_pulse:::: 
 *  //  startTM   = currentTM;
    //  currentTM = millis();
    //  if( currentTM > startTM ) {
    //    diffTM += currentTM - startTM;
    //    }
    //  // if enough time has passed to do a new reading...
    //  if(diffTM >= readTM) {// once reaching sampling period - save value & reset the ms counter
    //    temp_pulse_cnt = pulse_cnt;
    //    pulse_cnt = 0;
    //    diffTM = 0;
    //    }

 *  //unsigned long freq = temp_pulse_cnt * TSL_divider; //multiply counts by the sensors divide-by factor to solve for frequency
 *  
 *  8.9.: added a small filter to the HR-average-crossing-IF-loop [if(...&&(50<IRmax-IRmin))]
 *        better result with less artifact pulses
 *  
 *  10.09.: changed and made programm look more clean, different function order - NO influence recorded!
 *  
 *  12.09. - 22.09. work on implementing arrays to hold sensor values
 *  
 */
