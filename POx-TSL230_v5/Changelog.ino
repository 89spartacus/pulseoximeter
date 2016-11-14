/*
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
 */
