import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";
import { getAuth } from "firebase/auth";

const firebaseConfig = {
  apiKey: "AIzaSyCkVxnJ7TA3MgXOsDvDpxj8HGg-SY7NZzA",
  authDomain: "tcc-gabriel-9a0f9.firebaseapp.com",
  databaseURL: "https://tcc-gabriel-9a0f9-default-rtdb.firebaseio.com",
  projectId: "tcc-gabriel-9a0f9",
  storageBucket: "tcc-gabriel-9a0f9.firebasestorage.app",
  messagingSenderId: "444501722897",
  appId: "1:444501722897:web:bc7905d5f0af302d462eb0",
  measurementId: "G-35QHCSZG22"
};

const app = initializeApp(firebaseConfig);
export const db = getDatabase(app);
export const auth = getAuth(app);