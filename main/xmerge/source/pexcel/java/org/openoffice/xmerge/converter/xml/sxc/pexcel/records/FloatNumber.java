/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



package org.openoffice.xmerge.converter.xml.sxc.pexcel.records;

import java.io.OutputStream;
import java.io.InputStream;
import java.io.IOException;

import org.openoffice.xmerge.util.Debug;
import org.openoffice.xmerge.util.EndianConverter;
import org.openoffice.xmerge.converter.xml.sxc.pexcel.PocketExcelConstants;

/**
 * Represents a BIFF Record describing a floating point
 */
public class FloatNumber extends CellValue {

    protected byte[] num  = new byte[8];
    
	/**
 	 * Constructs a pocket Excel Document from the
 	 * <code>InputStream</code> and assigns it the document name passed in
	 *
 	 * @param	is InputStream containing a Pocket Excel Data file.
 	 */
    public FloatNumber(InputStream is) throws IOException {
    	read(is);
	}

	/**
 	 * Constructs a <code>FloatNumber</code> using specified attributes 
	 *
	 * @param row row number
	 * @param col column number 
	 * @param cellContents contents of the cell 
	 * @param ixfe font index
 	 */
    public FloatNumber(int row, int column, String cellContents, int ixfe) throws IOException {

		setIxfe(ixfe);
		setRow(row);
	   	setCol(column);
		double cellLong = (double) Double.parseDouble(cellContents);
		num 	= EndianConverter.writeDouble(cellLong);
	}

    /**
	 * Get the hex code for this particular <code>BIFFRecord</code> 
	 *
	 * @return the hex code for <code>FloatNumber</code>
	 */
    public short getBiffType() {
        return PocketExcelConstants.NUMBER_CELL;
    }

    /**
	 * Reads a<code>FloatNumber</code> from the specified <code>InputStream</code>
	 *
	 * @param input the <code>InputStram</code> to read from  
	 */    
    public int read(InputStream input) throws IOException {
	
		int numOfBytesRead = super.read(input);

        numOfBytesRead += input.read(num);    
        
        Debug.log(Debug.TRACE," num : " + getString());
        return numOfBytesRead;
    }
    
    public void write(OutputStream output) throws IOException {

    	output.write(getBiffType());
		
		super.write(output);

	    output.write(num);

		Debug.log(Debug.TRACE,"Writing FloatNumber record");
    }
   

    /**
	 * Gets the numerical value the cell represents 
	 *
	 * @return the <code>String</code> representing a double value 
	 */
	public String getString() throws IOException {

		double value = EndianConverter.readDouble(num);
		Double myDo = new Double(value);
		return myDo.toString();	
	}

}
