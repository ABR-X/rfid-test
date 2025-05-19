# Google Apps Script Setup Guide for RFID Attendance System

This guide will walk you through setting up the Google Apps Script that receives attendance data from your ESP8266 and logs it into a Google Sheet.

## Step 1: Create a New Google Sheet

1. Go to [Google Sheets](https://sheets.google.com)
2. Create a new blank spreadsheet
3. Rename it to something like "RFID Attendance System"

## Step 2: Open the Apps Script Editor

1. Click on "Extensions" in the top menu
2. Select "Apps Script"

This will open the Google Apps Script editor in a new tab.

## Step 3: Copy the Script Code

1. Delete any default code in the editor
2. Copy and paste the entire content from the `Google_Apps_Script.js` file
3. Click on the file name (default "Code.gs") and rename it to "RFIDAttendance"

## Step 4: Update the Sheet ID

1. Find the line with `const SHEET_ID = 'YOUR_SHEET_ID_HERE';`
2. Go back to your Google Sheet and copy the ID from the URL
   - The URL will look like: `https://docs.google.com/spreadsheets/d/YOUR_SHEET_ID_HERE/edit`
3. Paste your actual Sheet ID in the script, replacing 'YOUR_SHEET_ID_HERE'

## Step 5: Save the Script

1. Click on the disk icon or press Ctrl+S (Cmd+S on Mac) to save the script

## Step 6: Run the Setup Function

1. Select the `setupSpreadsheet` function from the dropdown menu next to the "Debug" button
2. Click the Run button (play icon)
3. You'll be asked to review permissions:
   - Click "Review permissions"
   - Select your Google account
   - Click "Advanced" and then "Go to RFIDAttendance (unsafe)"
   - Click "Allow"

After running this function, your Google Sheet should have two new sheets: "Attendance" and "Users", each with appropriate column headers.

## Step 7: Add Sample User (Optional)

1. Select the `addSampleUser` function from the dropdown
2. Click the Run button
3. This will add a sample user with Card ID "ABCD1234" to your Users sheet

## Step 8: Deploy as Web App

1. Click on "Deploy" in the top right
2. Select "New deployment"
3. Click the gear icon (⚙️) next to "Select type"
4. Choose "Web app"
5. Configure the following settings:
   - Description: "RFID Attendance System"
   - Execute as: "Me" (your email)
   - Who has access: "Anyone, even anonymous"
6. Click "Deploy"
7. Copy the Web app URL that appears
8. Replace the `scriptUrl` variable in your Arduino code with this URL

## Step 9: Verify the Deployment

1. Open the Web app URL in a new browser tab
2. You should see an error message like `{"status":"error","message":"No card ID provided"}`
3. This is normal because you haven't provided a card ID parameter

## Step 10: Add Real Users

1. Go back to your Google Sheet
2. Navigate to the "Users" sheet
3. Add rows for each user with:
   - Card ID: The RFID card's unique ID (you can scan cards with your system to get these)
   - User ID: Employee or student ID
   - Name: User's full name
   - Department: User's department (optional)
   - Notes: Any additional information (optional)

## Testing the System

To test if your Google Apps Script is working correctly:

1. Append a test parameter to your Web app URL:
   - `https://script.google.com/macros/s/.../exec?card_id=TESTCARD123&timestamp=2025-05-19T12:30:00`
2. Open this URL in your browser
3. You should see a success message
4. Check your Google Sheet to confirm that the test attendance was recorded

## Troubleshooting

- **403 Forbidden Error**: Make sure you've deployed as "Anyone, even anonymous"
- **Script not finding the sheet**: Verify the SHEET_ID is correct
- **Data not appearing in sheet**: Check the execution logs in the Apps Script editor

Remember that after making changes to the script, you need to create a new deployment to update the web app URL.