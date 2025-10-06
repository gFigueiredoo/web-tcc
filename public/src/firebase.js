import { initializeApp } from 'https://www.gstatic.com/firebasejs/10.7.1/firebase-app.js';
import { getDatabase, ref, onValue, set, push, remove } from 'https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js';

const firebaseConfig = {
  apiKey: "AIzaSyCkVxnJ7TA3MgXOsDvDpxj8HGg-SY7NZzA",
  authDomain: "tcc-gabriel-9a0f9.firebaseapp.com",
  databaseURL: "https://tcc-gabriel-9a0f9-default-rtdb.firebaseio.com",
  projectId: "tcc-gabriel-9a0f9",
  storageBucket: "tcc-gabriel-9a0f9.firebasestorage.app"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

export { db, ref, onValue, set, push, remove };