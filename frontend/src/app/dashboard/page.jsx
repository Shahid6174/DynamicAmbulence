"use client"
import React from 'react';
import TopNav from '../../../components/navbar.jsx';
import Sidebar from './sidebar.jsx';
import { useState } from "react"

import Stats from "./(subPages)/stats.jsx"
import Patients from "./(subPages)/patients.jsx"
import Appointments from "./(subPages)/appointments.jsx"
import Billing from "./(subPages)/billing.jsx"
import Doctors from "./(subPages)/doctors.jsx"
import Reports from "./(subPages)/reports.jsx"
import Settings from "./(subPages)/settings.jsx"

export default function MainApp (props){
  const [section, setSection] = useState(0)
  return (
    <div id="mainApp" className="page" >
      <TopNav />
      <div className="container-fluid">
        <div className="row">
          <Sidebar section = {section} setSection = {setSection}/>
          <main className="col-md-9 ms-sm-auto col-lg-10 px-md-4 py-4">
            <div id="dashboardPage" className="content-page">
              <h1 className="h2 text-primary mb-4">Welcome back, Admin!</h1>
              <div className="row g-4" id="statsCards">
                
              </div>
             
           

                {section === 0 && <Stats />}
                {section === 1 && <Patients/>}
                {section === 2 && < Appointments />}
                {section === 3 && < Doctors/>}
                {section === 4 && < Reports />}
                {section === 5 && < Billing/>}
                {section === 6 && < Settings />}
           
              

            </div>
          </main>
        </div>
      </div>
    </div>
  );
};


