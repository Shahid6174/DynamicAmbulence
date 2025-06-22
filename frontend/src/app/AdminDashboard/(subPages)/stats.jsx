'use client';

import { useState } from 'react';
import { Dropdown, Button, Collapse } from 'react-bootstrap';

export default function AnalyticsAndReports({ data = {} }) {
  const [stats, setStats] = useState({
    totalPatients: 0,
    emergencies: 0,
    avgDispatchTime: 0,
    avgCost: 0,
    aiEffectiveness: 0,
    hospitalLoad: 0,
    casualtyRate: 0,
    ...data,
  });

  const [expanded, setExpanded] = useState(null);

  const toggle = (section) =>
    setExpanded((prev) => (prev === section ? null : section));

  return (
    <div className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h4 className="fw-bold text-dark mb-0">
          <i className="bi bi-graph-up-arrow me-2"></i>Analytics & Reports
        </h4>
        <div className="d-flex align-items-center gap-3">
          <Dropdown>
            <Dropdown.Toggle size="sm" variant="outline-secondary">
              Filter: Today
            </Dropdown.Toggle>
            <Dropdown.Menu>
              <Dropdown.Item>Today</Dropdown.Item>
              <Dropdown.Item>This Week</Dropdown.Item>
              <Dropdown.Item>This Month</Dropdown.Item>
              <Dropdown.Item>Custom Range</Dropdown.Item>
            </Dropdown.Menu>
          </Dropdown>
          <Button size="sm" variant="outline-dark">
            <i className="bi bi-download me-1"></i>Download Report
          </Button>
        </div>
      </div>

      {/* KPI Cards */}
      <div className="row g-3 mb-4">
        <KPI label="Total Patients" icon="bi-people-fill" value={stats.totalPatients} color="primary" />
        <KPI label="Emergencies Today" icon="bi-exclamation-circle-fill" value={stats.emergencies} color="danger" />
        <KPI label="Avg Dispatch Time" icon="bi-clock-fill" value={`${stats.avgDispatchTime} min`} color="warning" />
        <KPI label="Avg Treatment Cost" icon="bi-cash-coin" value={`₹${stats.avgCost}`} color="success" />
        <KPI label="AI Effectiveness" icon="bi-cpu-fill" value={`${stats.aiEffectiveness}%`} color="info" />
        <KPI label="Hospital Load Index" icon="bi-hospital-fill" value={`${stats.hospitalLoad}%`} color="secondary" />
        <KPI label="Casualty Rate" icon="bi-emoji-frown-fill" value={`${stats.casualtyRate}%`} color="dark" />
      </div>

      {/* Expandable Detailed Sections */}
      <div className="accordion mb-4">
        {[
          {
            key: 'ai',
            title: 'AI Performance Insights',
            content: (
              <>
                <p>Effectiveness: <strong>{stats.aiEffectiveness}%</strong></p>
                <p>Top Missed Assignments: <em>None detected today</em></p>
                <p>Avg AI Decision Time: <strong>1.2s</strong></p>
              </>
            ),
          },
          {
            key: 'emergency',
            title: 'Emergency Response Breakdown',
            content: (
              <>
                <p>Total Emergencies: <strong>{stats.emergencies}</strong></p>
                <p>Avg Dispatch Delay: <strong>{stats.avgDispatchTime} min</strong></p>
                <p>Critical Cases: <strong>5</strong></p>
              </>
            ),
          },
          {
            key: 'cost',
            title: 'Cost Efficiency',
            content: (
              <>
                <p>Avg Cost per Patient: <strong>₹{stats.avgCost}</strong></p>
                <p>Total Cost Today: <strong>₹{stats.totalPatients * stats.avgCost}</strong></p>
              </>
            ),
          },
          {
            key: 'load',
            title: 'Hospital Load Overview',
            content: (
              <>
                <p>Load Index: <strong>{stats.hospitalLoad}%</strong></p>
                <p>Overloaded Hospitals: <strong>2</strong></p>
                <p>Avg Bed Utilization: <strong>78%</strong></p>
              </>
            ),
          },
        ].map(({ key, title, content }) => (
          <div className="card mb-2" key={key}>
            <div
              className="card-header bg-light d-flex justify-content-between align-items-center"
              onClick={() => toggle(key)}
              style={{ cursor: 'pointer' }}
            >
              <strong>{title}</strong>
              <i className={`bi ${expanded === key ? 'bi-chevron-up' : 'bi-chevron-down'}`}></i>
            </div>
            <Collapse in={expanded === key}>
              <div className="card-body">{content}</div>
            </Collapse>
          </div>
        ))}
      </div>
    </div>
  );
}

function KPI({ label, icon, value, color = 'primary' }) {
  return (
    <div className="col-md-4 col-lg-3">
      <div className={`card border-${color} shadow-sm h-100`}>
        <div className="card-body d-flex align-items-center justify-content-between">
          <div>
            <div className={`text-${color} fw-bold small`}>{label}</div>
            <div className="fs-5 fw-semibold">{value}</div>
          </div>
          <i className={`bi ${icon} fs-3 text-${color}`}></i>
        </div>
      </div>
    </div>
  );
}
