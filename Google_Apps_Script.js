// Google Apps Script for RFID Attendance System
// This script should be deployed as a web app with "Anyone, even anonymous" access

// Replace with your actual Google Sheet ID
const SHEET_ID = 'YOUR_SHEET_ID_HERE';

function doGet(e) {
  // Check if the required parameters are provided
  if (!e.parameter.card_id) {
    return ContentService.createTextOutput(JSON.stringify({
      'status': 'error',
      'message': 'No card ID provided'
    }));
  }
  
  try {
    // Get parameters
    const cardId = e.parameter.card_id;
    let timestamp = e.parameter.timestamp ? e.parameter.timestamp : new Date().toISOString();
    
    // Open the spreadsheet and the attendance sheet
    const ss = SpreadsheetApp.openById(SHEET_ID);
    const attendanceSheet = ss.getSheetByName('Attendance') || ss.insertSheet('Attendance');
    const usersSheet = ss.getSheetByName('Users') || ss.insertSheet('Users');
    
    // Find user information based on card ID
    const usersData = usersSheet.getDataRange().getValues();
    let userName = "Unknown User";
    let userId = "";
    
    for (let i = 1; i < usersData.length; i++) {  // Start at 1 to skip header row
      if (usersData[i][0] === cardId) {
        userId = usersData[i][1] || "";
        userName = usersData[i][2] || "Unknown User";
        break;
      }
    }
    
    // Get current date in YYYY-MM-DD format
    const today = new Date(timestamp).toISOString().split('T')[0];
    
    // Check if user already logged attendance today
    const attendanceData = attendanceSheet.getDataRange().getValues();
    for (let i = 1; i < attendanceData.length; i++) {  // Start at 1 to skip header row
      const rowDate = new Date(attendanceData[i][1]).toISOString().split('T')[0];
      if (attendanceData[i][0] === cardId && rowDate === today) {
        return ContentService.createTextOutput(JSON.stringify({
          'status': 'already',
          'message': 'Attendance already recorded today',
          'user': userName
        }));
      }
    }
    
    // Ensure headers exist
    if (attendanceSheet.getLastRow() === 0) {
      attendanceSheet.appendRow(['Card ID', 'Timestamp', 'User ID', 'Name']);
    }
    
    // Log the attendance
    attendanceSheet.appendRow([cardId, timestamp, userId, userName]);
    
    // Return success response
    return ContentService.createTextOutput(JSON.stringify({
      'status': 'success',
      'message': 'Attendance recorded successfully',
      'user': userName
    }));
    
  } catch (error) {
    // Return error response
    return ContentService.createTextOutput(JSON.stringify({
      'status': 'error',
      'message': 'Error: ' + error.toString()
    }));
  }
}

// Function to set up the spreadsheet (run this manually)
function setupSpreadsheet() {
  const ss = SpreadsheetApp.openById(SHEET_ID);
  
  // Set up Attendance sheet
  let attendanceSheet = ss.getSheetByName('Attendance');
  if (!attendanceSheet) {
    attendanceSheet = ss.insertSheet('Attendance');
    attendanceSheet.appendRow(['Card ID', 'Timestamp', 'User ID', 'Name']);
    attendanceSheet.setFrozenRows(1);
  }
  
  // Set up Users sheet
  let usersSheet = ss.getSheetByName('Users');
  if (!usersSheet) {
    usersSheet = ss.insertSheet('Users');
    usersSheet.appendRow(['Card ID', 'User ID', 'Name', 'Department', 'Notes']);
    usersSheet.setFrozenRows(1);
  }
  
  // Format the sheets
  [attendanceSheet, usersSheet].forEach(sheet => {
    // Auto-resize columns
    sheet.autoResizeColumns(1, sheet.getLastColumn());
    
    // Format header row
    const headerRange = sheet.getRange(1, 1, 1, sheet.getLastColumn());
    headerRange.setBackground('#4285F4');
    headerRange.setFontColor('#FFFFFF');
    headerRange.setFontWeight('bold');
  });
}

// Function to add a sample user (run this manually)
function addSampleUser() {
  const ss = SpreadsheetApp.openById(SHEET_ID);
  const usersSheet = ss.getSheetByName('Users') || ss.insertSheet('Users');
  
  // Check if headers exist
  if (usersSheet.getLastRow() === 0) {
    usersSheet.appendRow(['Card ID', 'User ID', 'Name', 'Department', 'Notes']);
  }
  
  // Add a sample user
  usersSheet.appendRow(['ABCD1234', 'EMP001', 'John Doe', 'Engineering', 'Sample user']);
}