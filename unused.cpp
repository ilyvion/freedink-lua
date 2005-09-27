/* Unused */
void reload_batch(void)
{
        FILE *stream;  
        char line[255];
        
        Msg("reoading .ini");     
        if (!exist("dink.ini")) 
        {
                Msg("File not found.");   
                
                //        sprintf(line,"Error finding the dink.ini file in the %s dir.",dir);
                //       TRACE(line);
                
        }
        
        
        
        
        if( (stream = fopen( "dink.ini", "r" )) != NULL )   
        {
                while(1)
                {
                        if( fgets( line, 255, stream ) == NULL) 
                                goto done;
                        else    
                        {
                                refigure_out(line);
                        }
                }
                
done:
                fclose( stream );  
        } else
        {
                //  TRACE("Dink.ini missing.");
        }
        
}
