"use client";
import React, { useState, useEffect } from "react";
import { useSearchParams } from 'next/navigation';

import TopNav from '../../../components/navbar.jsx';
import Sidebar from './sidebar.jsx';

import Stats from "./(subPages)/stats.jsx";
import Patients from "./(subPages)/patients.jsx";
import HospitalStatus from "./(subPages)/hospitalstatus.jsx";
import SendFeed from "./(subPages)/sendfeedback.jsx";
import Staff from "./(subPages)/hospitalstaff.jsx";

export default function MainApp() {
  const [section, setSection] = useState(0);
  const searchParams = useSearchParams();
  const hospitalId = searchParams.get('hospitalId');
  const [hospital, setHospital] = useState(null);

  useEffect(() => {
    if (hospitalId) {
      fetch(`/api/mongo?collection=Hospital&q=${JSON.stringify({ _id: { "$oid": hospitalId } })}`)
        .then(res => res.json())
        .then(data => {
          if (data?.data?.length) {
            setHospital(data.data[0]);
          }
        })
        .catch(console.error);
    }
  }, [hospitalId]);

  return (
    <div id="mainApp" className="page">
      <TopNav />
      <div className="container-fluid">
        <div className="row">
          <Sidebar section={section} setSection={setSection} />
          <main className="col-md-9 ms-sm-auto col-lg-10 px-md-4 py-4">
            <div id="dashboardPage" className="content-page">
              <h1 className="h2 text-primary mb-4">
                Welcome back, Hospital Admin {hospital ? `from ${hospital.name}` : '👋'}!
              </h1>

              <div className="row g-4" id="statsCards"></div>

              {section === 0 && <Stats />}
              {section === 1 && <Patients />}
              {section === 2 && <HospitalStatus />}
              {section === 3 && <SendFeed />}
              {section === 4 && <Staff />}
            </div>
          </main>
        </div>
      </div>
    </div>
  );
}
